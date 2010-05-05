#ifndef CLASSMESHMANAGER
#define CLASSMESHMANAGER

#define NIFLIB_STATIC_LINK
#include <niflib.h>
#include <obj/NiNode.h>
#include <obj/NiTriStrips.h>
#include <obj/NiTriStripsData.h>
#include <obj/NiTexturingProperty.h>
#include <obj/NiTriShape.h>
#include <obj/NiTriShapeData.h>


using namespace Niflib;

typedef struct
{
    D3DXVECTOR3 Pos;
	D3DXVECTOR3 Norm;
	D3DXVECTOR4 Color;
	D3DXVECTOR2 TextUV;
} MeshVertex;

typedef struct
{
} MeshIndiceBuf;

typedef struct
{
	ID3D10Buffer *pVertexBuf;
	unsigned long numvert;
	ID3D10Buffer *pIndiceBuf;
	unsigned long numind;
	CTexture *datext; // texture for this part
	float local_translation[3]; //additional translation
	unsigned long tristrip; //are those triangle strips
} MeshData;

class CMesh
{
public:
	CMesh();
	~CMesh();
	unsigned long Load_Mesh(unsigned long);

	unsigned long Get_FormID();

	void Draw(float *,float *,float);
protected:
	MeshData *AddTriStrips(NiTriStripsRef);
	MeshData *AddTriShape(NiTriShapeRef);
protected:
	unsigned long formID;
	vector<MeshData *> listdata;
	CTexture *MeshTexture;
};

class CMeshManager
{
public:
	CMeshManager();
	~CMeshManager();
	//Load a STAT object
	CMesh *Load_STAT(unsigned long);
protected:
	vector<CMesh *> listmeshes;
};

#endif