#include "stdafx.h"
// Keycode
#include <Common/Base/keycode.cxx>
#include "resource.h"
#include "DDBlivion.h"
#include "CGlobal.h"
#include "CLandManager.h"
#include "CInputManager.h"
#include "Global.h"

#include "CHavokManager.h"

static void HK_CALL output_havok_error(const char* msg, void* userArgGivenToInit)
{
	unsigned long length;
	WCHAR temp_d[1024];
	WCHAR temp_s[512];

	memset(temp_s,0,512);
	MultiByteToWideChar(CP_ACP,0,msg,-1,temp_s,512);

	length=GetWindowTextLength(GetDlgItem(g_debug_hwnd,IDC_EDIT_DEBUG));

	memset(temp_d,0,1024);
	swprintf_s(temp_d,1024,L"Havok Error Message: %s\r\n",temp_s);
	SendDlgItemMessage(g_debug_hwnd,IDC_EDIT_DEBUG,EM_SETSEL,length,length);
	SendDlgItemMessage(g_debug_hwnd,IDC_EDIT_DEBUG,EM_REPLACESEL,0,(LPARAM)temp_d);
	process_messages();
}


CHavokManager::CHavokManager()
{
	crit_hkWorld=NULL;

	crit_hkWorld=new CRITICAL_SECTION;
	if(!InitializeCriticalSectionAndSpinCount(crit_hkWorld,4000))
	{
		CLEAN_DELETE(crit_hkWorld);
		throw L"Couldn't init crit_hkWorld...";
	}
}

CHavokManager::~CHavokManager()
{
	if(crit_hkWorld)
	{
		DeleteCriticalSection(crit_hkWorld);
		CLEAN_DELETE(crit_hkWorld);
	}
}

void CHavokManager::Init()
{
	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault();
	hkBaseSystem::init(memoryRouter,output_havok_error);
	//Init multithreading
	int totalNumThreadsUsed;

	//Get the number of hardware threads available
	hkHardwareInfo hwInfo;
	hkGetHardwareInfo(hwInfo);
	totalNumThreadsUsed = hwInfo.m_numThreads;
	hkCpuJobThreadPoolCinfo threadPoolCinfo;
	//Num of hardware threads -1
	threadPoolCinfo.m_numThreads = totalNumThreadsUsed - 2;
	//No Timer Info
	threadPoolCinfo.m_timerBufferPerThreadAllocation = 0;
	threadPool = new hkCpuJobThreadPool( threadPoolCinfo );

	hkJobQueueCinfo info;
	info.m_jobQueueHwSetup.m_numCpuThreads = totalNumThreadsUsed;
	jobQueue = new hkJobQueue(info);

	//Init the Physics world
	hkpWorldCinfo worldInfo;
	
	worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED; //Yep we're going multithreaded
	worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_REMOVE_ENTITY; //Remove entity if outside the broadphase
	worldInfo.setBroadPhaseWorldSize(12000.0f); //Size of the cube surrounding the character where physics are active
	worldInfo.m_gravity.set(0.0f,0.0f,-9.8f);
	worldInfo.m_collisionTolerance = 0.1f;

	physicsWorld = new hkpWorld(worldInfo);
	physicsWorld->markForWrite();
	physicsWorld->m_wantDeactivation = true;

	//Enable all collision agent types
	hkpAgentRegisterUtil::registerAllAgents(physicsWorld->getCollisionDispatcher());

	physicsWorld->registerWithJobQueue( jobQueue );
	physicsWorld->unmarkForWrite();
}

void CHavokManager::SetupCharacterProxy()
{
	//Lock the world while we update
	//physicsWorld->lock();
	physicsWorld->markForWrite();

	hkVector4 vertexA(0,0, 0.4f); //Top of character
	hkVector4 vertexB(0,0,-0.4f); //Bottom of character

	//Create a capsule to represent the character standing(with the radius the size is 128.0f)
	m_standShape = new hkpCapsuleShape(vertexA, vertexB, 0.6f);

	m_char_phantom = new hkpSimpleShapePhantom( m_standShape, hkTransform::getIdentity(), hkpGroupFilter::calcFilterInfo(0,2) );

	physicsWorld->addPhantom(m_char_phantom);
	m_char_phantom->removeReference();

	hkpCharacterProxyCinfo cpci;
	float charpos[3];
	pGlobal->GetPlayerPositionHavok(charpos);
	cpci.m_position.set(charpos[0],charpos[1],charpos[2]);

	cpci.m_staticFriction = 0.0f;
	cpci.m_dynamicFriction = 1.0f;
	cpci.m_up.setNeg4( physicsWorld->getGravity() );
	cpci.m_up.normalize3();	
	cpci.m_userPlanes = 4;
	cpci.m_maxSlope = HK_REAL_PI / 3.f;

	cpci.m_shapePhantom = m_char_phantom;
	m_characterProxy = new hkpCharacterProxy( cpci );

	hkpCharacterState* state;
	hkpCharacterStateManager* manager = new hkpCharacterStateManager();

	state = new hkpCharacterStateOnGround();
	(static_cast<hkpCharacterStateOnGround *>(state))->setSpeed(20.0f);
	manager->registerState(state,HK_CHARACTER_ON_GROUND);
	state->removeReference();

	state = new hkpCharacterStateInAir();
	manager->registerState(state,HK_CHARACTER_IN_AIR);
	state->removeReference();

	state = new hkpCharacterStateJumping();
	manager->registerState(state,HK_CHARACTER_JUMPING);
	state->removeReference();

	state = new hkpCharacterStateJumping();
	manager->registerState(state,HK_CHARACTER_CLIMBING);
	state->removeReference();

	m_characterContext = new hkpCharacterContext(manager, HK_CHARACTER_IN_AIR);
	manager->removeReference();

	physicsWorld->unmarkForWrite();
	//physicsWorld->unlock();
}

hkpRigidBody *CHavokManager::CreateLandscapeRigid(float *height_data,float x,float y)
{
	HavokSampledHeightMap *sampHeightMapShape;

	hkpSampledHeightFieldBaseCinfo ci;
	ci.m_xRes = 33;
	ci.m_zRes = 33;
	ci.m_scale.set(SCALE/SCALE_HAVOK,-(1.0f/SCALE_HAVOK),SCALE/SCALE_HAVOK);
	sampHeightMapShape = new HavokSampledHeightMap(ci,height_data);

	hkpTriSampledHeightFieldCollection* collection = new hkpTriSampledHeightFieldCollection( sampHeightMapShape );
	hkpTriSampledHeightFieldBvTreeShape* bvTree = new hkpTriSampledHeightFieldBvTreeShape( collection );

	hkpRigidBodyCinfo rci;
	rci.m_motionType = hkpMotion::MOTION_FIXED;
	rci.m_friction = 0.2f;
	rci.m_shape = bvTree;

	hkVector4 davec;
	davec.setZero4();
	davec(0)=1;
	rci.m_rotation.setAxisAngle(davec,-(HK_REAL_PI/2.f));

	rci.m_position.set(x,y,0);

	hkpRigidBody* rigidLandscape = new hkpRigidBody( rci );

	sampHeightMapShape->removeReference();
	collection->removeReference();
	bvTree->removeReference();

	return rigidLandscape;
}

void CHavokManager::AddRigidBody(hkpRigidBody *toadd)
{
	EnterCriticalSection(crit_hkWorld);
	//physicsWorld->lock();
	physicsWorld->markForWrite();
	physicsWorld->addEntity(toadd);
	physicsWorld->unmarkForWrite();
	//physicsWorld->unlock();
	LeaveCriticalSection(crit_hkWorld);
}

void CHavokManager::CreateThreadMemoryRouter(hkMemoryRouter &memoryRouter)
{
	hkMemorySystem::getInstance().threadInit( memoryRouter, "hkCpuJobThreadPool" );
	hkBaseSystem::initThread(&memoryRouter);
}

void CHavokManager::UpdateHavokState()
{
	hkpCharacterInput input;
	hkpCharacterOutput output;
	hkVector4 up;
	hkQuaternion orient;

	up.setZero4();
	up(2)=1;

	orient.setAxisAngle(up, pGlobal->GetYaw());

	EnterCriticalSection(crit_hkWorld);
	physicsWorld->lock();
	physicsWorld->markForWrite();

	input.m_inputLR = 0.0f;
	input.m_inputUD = 0.0f;
	input.m_wantJump=false;
	if(pInputManager->IsKeyPressed(DIK_A)) input.m_inputLR=1.0f;
	if(pInputManager->IsKeyPressed(DIK_D)) input.m_inputLR=-1.0f;
	if(pInputManager->IsKeyPressed(DIK_W)) input.m_inputUD=-1.0f;
	if(pInputManager->IsKeyPressed(DIK_S)) input.m_inputUD=1.0f;
	if(pInputManager->IsKeyPressed(DIK_SPACE)) input.m_wantJump=true;
	input.m_atLadder=false;
	input.m_up=up;
	input.m_forward.set(0,1,0);
	input.m_forward.setRotatedDir(orient,input.m_forward);

	input.m_stepInfo.m_deltaTime=pGlobal->GetFrameTime();
	input.m_stepInfo.m_invDeltaTime=1.0f/pGlobal->GetFrameTime();
	input.m_characterGravity.set(0,0,-16);
	input.m_velocity=m_characterProxy->getLinearVelocity();
	input.m_position=m_characterProxy->getPosition();

	hkVector4 down;	down.setNeg4(up);
	m_characterProxy->checkSupport(down, input.m_surfaceInfo);

	/*switch(input.m_surfaceInfo.m_supportedState)
	{
	case hkpSurfaceInfo::SUPPORTED:
		output_text(L"Supported\r\n");
		break;
	case hkpSurfaceInfo::UNSUPPORTED:
		output_text(L"Unsupported\r\n");
		break;
	case hkpSurfaceInfo::SLIDING:
		output_text(L"Sliding\r\n");
		break;
	default:
		output_text(L"Unknown...\r\n");
		break;
	}*/

	m_characterContext->update(input, output);

	hkStepInfo si;
	si.m_deltaTime = pGlobal->GetFrameTime();
	si.m_invDeltaTime = 1.0f/pGlobal->GetFrameTime();
	m_characterProxy->setLinearVelocity(output.m_velocity);
	m_characterProxy->integrate( si, physicsWorld->getGravity() );

	/*hkpCharacterStateType state = m_characterContext->getState();
	switch (state)
	{
	case HK_CHARACTER_ON_GROUND:
		output_text(L"On Ground");	break;
	case HK_CHARACTER_JUMPING:
		output_text(L"Jumping"); break;
	case HK_CHARACTER_IN_AIR:
		output_text(L"In Air"); break;
	case HK_CHARACTER_CLIMBING:
		output_text(L"Climbing"); break;
	default:
		output_text(L"Other");	break;
	}*/


	physicsWorld->unmarkForWrite();
	physicsWorld->unlock();

	//Step the world
	physicsWorld->stepMultithreaded(jobQueue,threadPool,pGlobal->GetFrameTime());

	//Get the character position
	physicsWorld->markForWrite();
	//physicsWorld->lock();
	hkVector4 curpos;
	float fcurpos[3];
	curpos=m_characterProxy->getPosition();
	fcurpos[0]=curpos(0);
	fcurpos[1]=curpos(1);
	fcurpos[2]=curpos(2);
	pGlobal->SetPlayerPositionHavok(fcurpos);
	physicsWorld->unmarkForWrite();
	//physicsWorld->unlock();
	LeaveCriticalSection(crit_hkWorld);
}
