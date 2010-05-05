#ifndef CLASSLANDMANAGER
#define CLASSLANDMANAGER

#include "CTextureManager.h"
#include "CMeshManager.h"
#include "CTreeManager.h"

#define LANDOFFSET_Y 128
#define LANDOFFSET_X 128
#define RENDERCELLRADIUS 8
#define RENDERCLOSEMESHRADIUS 4
#define HAVOKACTIVECELLS 4

#define CELLSIZE 4096.0f
#define SCALE (4096.0f/32.0f)
#define SCALEHEIGHT 8.0f
#define SCALETEXT 0.5f

typedef struct
{
    D3DXVECTOR3 Pos;
	D3DXVECTOR3 Norm;
	D3DXVECTOR4 Color;
	D3DXVECTOR2 TextUV;
	float weights[9];
} VertexLand;

extern ID3D10Buffer *pLandIndices;

typedef struct
{
	CMesh *damesh;
	float *pos_and_rot;
	float scale;
} Mesh_Refr;

class CLand
{
public:
	CLand();
	~CLand();
public:
	void Load(CCell_Record *);
	void LoadDistantMeshes();
	void LoadCloseMeshes();

	void Get_XY(long &,long &);
	
	void ActivatePhysics();

	void CheckAgainstFrustrum();
	bool IsVisible();

	void Draw();
	void DrawCloseMeshes();
	void DrawDistantMeshes();

protected:
	void SetNormals(char *);
	void SetPositionsAndHeights(float,char *);
	void SetColors(unsigned char *);
	void SetBaseTexture(unsigned long,unsigned char);
	void SetAlphaTexture(unsigned long,unsigned char,unsigned short);
	void ProcessVTXT(unsigned long,float *,unsigned short *,unsigned short,unsigned char);
	void CalculateBaseOpacities();
protected:
	VertexLand *VertexBufs[4];
	ID3D10Buffer *pFinalBufs[4];
	CTexture *pTextureList[4][9];
	//Heightmap(we keep it for havok use)
	float tabHeight[(33*33)];
	//Position of the landscape
	long x,y;
	//FormID of the landscape
	unsigned long formID;
	//rigid body corresponding to the landscape for havok(we keep it here so we can activate it when needed)
	hkpRigidBody *landBody;
	//List of close meshes
	vector<Mesh_Refr *> listclosemeshes;
	//List of distant meshes
	vector<Mesh_Refr *> listdistantmeshes;
	//The cell record associated
	CCell_Record *the_cell;
	//4 border points to do some very simple culling(testing)
	float border_points[4][3];
	bool frustrum_visible;

	//For multithreading
	CRITICAL_SECTION *crit_mesh_loaded;
	//Are the meshes loaded?
	bool close_meshes_loaded;
	bool distant_meshes_loaded;
};

typedef struct _quadtree_entry
{
	long pos[2];
	unsigned long size;


class CLandManager
{
public:
	CLandManager();
	~CLandManager();
	//Init basic stuff(should be done in the constructor maybe?)
	void Init();
	//Draw the landscape around the player
	void Draw();
	//Load the landscape for the first time(CGlobal::SetPlayerPosition must have been called as well a CTreeManager::SetWorldspace)
	void InitWorld();
	//Update the view(called by the streamer thread)
	void UpdateInternalView(long,long);
protected:
	//List of CLand used by the main thread
	CLand *listLands[256][256];
	long curcellx,curcelly;
	//Stuff used for multithreading
	CRITICAL_SECTION *crit_ListLands;
	DWORD Thread_StreamLoader_ID;
	HANDLE StreamLoaderHandle;
	CLand *internalLands[256][256];
};

#endif