#ifndef HAVOKMANAGER
#define HAVOKMANAGER

class HavokSampledHeightMap : public hkpSampledHeightFieldShape
{
public:
	HavokSampledHeightMap(const hkpSampledHeightFieldBaseCinfo& ci,float *_land_heights) : hkpSampledHeightFieldShape(ci)
	{
		land_heights=_land_heights;
	}
	virtual void collideSpheres( const CollideSpheresInput& input, SphereCollisionOutput* outputArray) const
	{
		return hkSampledHeightFieldShape_collideSpheres(*this, input, outputArray);
	}
	HK_FORCE_INLINE hkReal getHeightAtImpl( int x, int z ) const
	{
		return land_heights[(z*m_xRes)+x];
	}
	HK_FORCE_INLINE hkBool getTriangleFlipImpl() const { return false; }
protected:
	float *land_heights;
};

class CHavokManager
{
public:
	//Constructors and Destructors(empty atm)
	CHavokManager();
	~CHavokManager();
	//The initialization
	void Init();
	//Step the physics world	
	void UpdateHavokState();
	//Add a rigid body to the physics world
	void AddRigidBody(hkpRigidBody *);
	//Setup the character
	void SetupCharacterProxy();

	//Create a rigid body from landscape
	hkpRigidBody *CreateLandscapeRigid(float *,float,float);

	//Create a hkMemoryRouter for a thread
	void CreateThreadMemoryRouter(hkMemoryRouter &);
protected:
	//Hardware havok stuff
	hkJobThreadPool* threadPool;
	hkJobQueue* jobQueue;

	//The magical world where physics become true
	hkpWorld* physicsWorld;
	
	//The phantom of the character
	hkpShapePhantom* m_char_phantom;
	//The character proxy
	hkpCharacterProxy* m_characterProxy;
	//Shape of standing character
	hkpShape* m_standShape;
	//Not sure what this is...
	hkpCharacterContext* m_characterContext;
	//Critical section for the havok world
	CRITICAL_SECTION *crit_hkWorld;
};

#endif