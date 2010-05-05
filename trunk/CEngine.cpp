#include "stdafx.h"
#include "DDBlivion.h"
#include "Global.h"
#include "CDirectXManager.h"
#include "CInputManager.h"
#include "CTreeManager.h"
#include "CMeshManager.h"
#include "CHavokManager.h"
#include "CGlobal.h"
#include "CEngine.h"

//Global Tree
CTree_Manager *pTreeManager=NULL;
CDirectXManager *pDXManager=NULL;
CTextureManager *pTextureManager=NULL;
CInputManager *pInputManager=NULL;
CMeshManager *pMeshManager=NULL;
CHavokManager *pHavokManager=NULL;
CWeatherManager *pWeatherManager=NULL;
CGlobal *pGlobal;

extern HWND g_debug_hwnd;

CEngine::CEngine()
{
	pTreeManager=NULL;
	pDXManager=NULL;
	pLandManager=NULL;
	pInputManager=NULL;
	pMeshManager=NULL;
	pTextureManager=NULL;
	pHavokManager=NULL;
	pWeatherManager=NULL;
	pGlobal=NULL;
}

CEngine::~CEngine()
{
	CLEAN_DELETE(pTreeManager);
	CLEAN_DELETE(pLandManager);
	CLEAN_DELETE(pInputManager);
	CLEAN_DELETE(pWeatherManager);
	CLEAN_DELETE(pMeshManager);
	CLEAN_DELETE(pHavokManager);
	CLEAN_DELETE(pTextureManager);
	CLEAN_DELETE(pDXManager);
	CLEAN_DELETE(pGlobal);
}

void CEngine::Init()
{
	WCHAR temp_s[1024];

	try {
		//Let's initialize DirectX
		output_text(L"Initializing CDirectXManager...\r\n");
		pDXManager=new CDirectXManager();
		//Set up the render window
		output_text(L"Creating Render Window...\r\n");
		pDXManager->CreateRenderWindow(g_debug_hwnd);
		//Initialize
		output_text(L"Initializing Device...\r\n");
		pDXManager->InitDevice();
		output_text(L"Preparing DX...\r\n");
		pDXManager->Prepare_DX();
		
		//Initialize Input
		output_text(L"Initializing CInputManager...\r\n");
		pInputManager=new CInputManager();
		pInputManager->Init(g_directx_hwnd);

		//Init the Tree Manager
		output_text(L"Initializing CTree_Manager...\r\n");
		pTreeManager=new CTree_Manager();

		//Init the Mod Manager
		output_text(L"Initializing CModManager...\r\n");
		pModManager=new CModManager();
		pModManager->Init();

		//Load all the mods
		output_text(L"Loading all the mods...\r\n");
		unsigned long timea,timeb;

		timea=GetTickCount();
		pModManager->LoadAllMods();
		timeb=GetTickCount();

		memset(temp_s,0,1024);
		swprintf_s(temp_s,1024,L"Load order parsed in %d milliseconds\r\n",(timeb-timea));
		output_text(temp_s);

		//Optimize the tree for faster access(needed)
		output_text(L"Optimizing the tree...\r\n");
		pTreeManager->Optimize_Tree();

		//Init the Texture Manager
		output_text(L"Initializing CTextureManager...\r\n");
		pTextureManager=new CTextureManager();

		//Init the Global Manager
		output_text(L"Initializing CGlobal...\r\n");
		pGlobal=new CGlobal();

		//Init the Havok Manager
		output_text(L"Initializing CHavokManager...\r\n");
		pHavokManager=new CHavokManager();
		pHavokManager->Init();

		//Init the Mesh manager
		output_text(L"Initializing CMeshManager...\r\n");
		pMeshManager=new CMeshManager();

		//Mods loaded, time to init the landscape manager
		output_text(L"Initializing CLandManager...\r\n");
		pLandManager=new CLandManager();
		pLandManager->Init();

		//Init the weather manager
		output_text(L"Initializing CWeatherManager...\r\n");
		pWeatherManager=new CWeatherManager();
		pWeatherManager->Init();
		
		//The rest should be handled by save/new game/menu stuff but for testing it's there right now
		//Let's go to Tamriel!
		output_text(L"Setting Worldspace to Tamriel...\r\n");
		pTreeManager->SetWorldspace(0x3C);
		//pTreeManager->SetWorldspace(0x9F18);

		//Set the position
		float tpos[3] = { 17210.765625f, 66045.656250f, 5044.993164f };
		pGlobal->SetPlayerPosition(tpos);

		//Load appropriate landscape and create the threaded loader
		pLandManager->InitWorld();

		//Add the character proxy to the scene
		pHavokManager->SetupCharacterProxy();

		output_text(L"Starting render!\r\n");
		//Initialize the time keeper
		pGlobal->UpdateTimeKeeper();
	}
	catch(WCHAR *wcharexception)
	{
		output_text(L"Initialization Exception:");
		output_text(wcharexception);
		output_text(L"\r\n");
		return;
	}
	catch(...)
	{
		output_text(L"Unknown exception occured!\r\n");
		return;
	}

	long framecount=0;

	try {
		//Let's loop baby!!
		MSG msg = {0};
		while( WM_QUIT != msg.message )
		{
	        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
	            TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				//Update the input
				pInputManager->UpdateInput();
				//Update the camera
				pDXManager->UpdateCamera();
				//Start the rendering(clean buffers)
				pDXManager->StartRender();
				//Draw the landscape
				pLandManager->Draw();
				//Present the buffer
				pDXManager->EndRender();
				//Update the time keeper
				pGlobal->UpdateTimeKeeper();
				//Update havok
				pHavokManager->UpdateHavokState();

				//Escape if esc is pressed
				if(pInputManager->IsKeyPressed(DIK_ESCAPE)) PostQuitMessage(0);
				framecount++;
				if(framecount%600==0)
				{
					float tempf[3];
					WCHAR tempst[500];
					output_text_value(L"Number of frames rendered:",framecount);
					pGlobal->GetPlayerPosition(tempf);
					swprintf_s(tempst,500,L"PosX:%f PosY:%f PosZ:%f\r\n",tempf[0],tempf[1],tempf[2]);
					output_text(tempst);
				}
			}
		}
	}
	catch(WCHAR *wcharexception)
	{
		output_text(L"Exception running the program:");
		output_text(wcharexception);
		output_text(L"\r\n");
	}
	catch(...)
	{
		output_text(L"Unknown exception occured!\r\n");
		return;
	}

	output_text(L"We're finished!\r\n");
}