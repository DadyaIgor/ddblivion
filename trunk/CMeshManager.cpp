#include "stdafx.h"
#include "global.h"
#include "CTextureManager.h"
#include "CTreeManager.h"

#define NIFLIB_STATIC_LINK
#include <niflib.h>
#include <obj/NiNode.h>
#include <obj/NiTriStrips.h>
#include <obj/NiTexturingProperty.h>
#include <obj/NiTriStripsData.h>
#include <obj/NiGeometryData.h>
#include <obj/NiSourceTexture.h>
#include <obj/NiCollisionObject.h>
#include <obj/bhkCollisionObject.h>
#include <obj/NiObject.h>
#include <obj/bhkRigidBody.h>
#include <gen/TexDesc.h>

#include "CMeshManager.h"

using namespace Niflib;

CMesh::CMesh()
{
}

CMesh::~CMesh()
{
}

MeshData *CMesh::AddTriStrips(NiTriStripsRef tristripsnode)
{
	unsigned long index;
	MeshData *newmeshpart;
	MeshVertex *davert;
	unsigned short *daind;

	if(tristripsnode==NULL) return NULL;

	NiGeometryDataRef ergeom = tristripsnode->GetData();
	NiTriStripsDataRef geom = DynamicCast<NiTriStripsData>(ergeom);
	if(geom==NULL)
	{
		output_text(L"Couldn't cast to NiTriStripsData!\r\n");
		return NULL;
	}


	if(geom->GetUVSetCount()==0||geom->GetStripCount()==0)
	{
		if(geom->GetUVSetCount()==0) output_text_hex(L"No UV Set:",formID);
		if(geom->GetStripCount()==0) output_text_hex(L"No Strip:",formID);
		return NULL;
	}

	
	//Get the vertices
	vector<Vector3> davertices=geom->GetVertices();
	//Get the uvs
	vector<TexCoord> dauvs=geom->GetUVSet(0);
	//Get the normals
	vector<Vector3> danormals=geom->GetNormals();
	//Get the colors
	vector<Color4> dacolors=geom->GetColors();
	//Get the indices
	vector<unsigned short> daindices=geom->GetStrip(0);

	newmeshpart=new MeshData;

	//Get the number of vertex
	newmeshpart->numvert=geom->GetVertexCount();
	//Get the number of indices
	newmeshpart->numind=daindices.size();
	//newmeshpart->numind=geom->GetVertexIndexCount();

	//output_text_value(L"Number of vertex:",newmeshpart->numvert);
	//output_text_value(L"Number of indices:",newmeshpart->numind);

	if(newmeshpart->numvert==0||newmeshpart->numind==0)
	{
		if(newmeshpart->numvert==0) output_text_hex(L"No vertex?:",formID);
		if(newmeshpart->numind==0) output_text_hex(L"No indices?:",formID);
		delete newmeshpart;
		return NULL;
	}

	davert=new MeshVertex[newmeshpart->numvert];
	daind=new unsigned short[newmeshpart->numind];

	for(index=0;index<newmeshpart->numind;index++)
	{
		//memcpy later
		//daind[index]=daindices[(newmeshpart->numind-1)-index];
		daind[index]=daindices[index];
	}

	for(index=0;index<newmeshpart->numvert;index++)
	{
		//change to use memcpy later
		davert[index].Pos.x=davertices[index].x;
		davert[index].Pos.y=davertices[index].y;
		davert[index].Pos.z=davertices[index].z;
		davert[index].TextUV.x=dauvs[index].u;
		davert[index].TextUV.y=dauvs[index].v;
		davert[index].Norm.x=danormals[index].x;
		davert[index].Norm.y=danormals[index].y;
		davert[index].Norm.z=danormals[index].z;
	}

	if(dacolors.size()!=0)
	{
		for(index=0;index<newmeshpart->numvert;index++)
		{
			davert[index].Color.x=dacolors[index].r;
			davert[index].Color.y=dacolors[index].g;
			davert[index].Color.z=dacolors[index].b;
			davert[index].Color.w=dacolors[index].a;
		}
	}
	else
	{
		for(index=0;index<newmeshpart->numvert;index++)
		{
			davert[index].Color.x=1.0f;
			davert[index].Color.y=1.0f;
			davert[index].Color.z=1.0f;
			davert[index].Color.w=1.0f;
		}
	}

	//Load the texture
	vector<NiPropertyRef> daprops=tristripsnode->GetProperties();
	bool foundtext=false;
	for(index=0;index<daprops.size();index++)
	{
		if(daprops[index]->IsSameType(NiTexturingProperty::TYPE))
		{
			NiTexturingPropertyRef datextprop=DynamicCast<NiTexturingProperty>(daprops[index]);
			if(datextprop==NULL)
			{
				output_text(L"Failed NiTexturingProperty Cast!\r\n");
				delete newmeshpart;
				delete [] davert;
				delete [] daind;
				return NULL;
			}
			foundtext=true;
			TexDesc datextdesc=datextprop->GetTexture(0);
			newmeshpart->datext=pTextureManager->Load_Direct((char *)datextdesc.source->GetTextureFileName().c_str());
			break;
		}
	}

	if(foundtext==false) output_text_hex(L"Couldn't find texture:",formID);

	newmeshpart->pIndiceBuf=pDXManager->CreateIndiceBuffer((unsigned char *)daind,newmeshpart->numind,sizeof(unsigned short));
	newmeshpart->pVertexBuf=pDXManager->CreateVertexBuffer((unsigned char *)davert,newmeshpart->numvert,sizeof(MeshVertex));

	delete [] daind;
	delete [] davert;
	
	//Ok now let's add it
	listdata.push_back(newmeshpart);

	return newmeshpart;
}

MeshData *CMesh::AddTriShape(NiTriShapeRef trishapenode)
{
	unsigned long index;
	MeshData *newmeshpart;
	MeshVertex *davert;
	unsigned short *daind;

	if(trishapenode==NULL) return NULL;

	NiGeometryDataRef ergeom = trishapenode->GetData();
	NiTriShapeDataRef geom = DynamicCast<NiTriShapeData>(ergeom);
	if(geom==NULL)
	{
		output_text(L"Couldn't cast to NiTriStripsData!\r\n");
		return NULL;
	}


	if(geom->GetUVSetCount()==0||geom->GetTriangles().size()==0)
	{
		if(geom->GetUVSetCount()==0) output_text_hex(L"No UV Set:",formID);
		if(geom->GetTriangles().size()==0) output_text_hex(L"No Triangles:",formID);
		return NULL;
	}
	
	//Get the vertices
	vector<Vector3> davertices=geom->GetVertices();
	//Get the uvs
	vector<TexCoord> dauvs=geom->GetUVSet(0);
	//Get the normals
	vector<Vector3> danormals=geom->GetNormals();
	//Get the colors
	vector<Color4> dacolors=geom->GetColors();
	//Get the indices
	vector<Triangle> daindices=geom->GetTriangles();

	newmeshpart=new MeshData;

	//Get the number of vertex
	newmeshpart->numvert=geom->GetVertexCount();
	//Get the number of indices
	newmeshpart->numind=daindices.size()*3;
	//newmeshpart->numind=geom->GetVertexIndexCount();

	//output_text_value(L"Number of vertex:",newmeshpart->numvert);
	//output_text_value(L"Number of indices:",newmeshpart->numind);

	if(newmeshpart->numvert==0||newmeshpart->numind==0)
	{
		if(newmeshpart->numvert==0) output_text_hex(L"No vertex?:",formID);
		if(newmeshpart->numind==0) output_text_hex(L"No indices?:",formID);
		delete newmeshpart;
		return NULL;
	}

	davert=new MeshVertex[newmeshpart->numvert];
	daind=new unsigned short[newmeshpart->numind];

	for(index=0;index<(newmeshpart->numind/3);index++)
	{
		//memcpy later
		//daind[index]=daindices[(newmeshpart->numind-1)-index];
		daind[index*3]=daindices[index].v1;
		daind[(index*3)+1]=daindices[index].v2;
		daind[(index*3)+2]=daindices[index].v3;
		
	}

	for(index=0;index<newmeshpart->numvert;index++)
	{
		//change to use memcpy later
		davert[index].Pos.x=davertices[index].x;
		davert[index].Pos.y=davertices[index].y;
		davert[index].Pos.z=davertices[index].z;
		davert[index].TextUV.x=dauvs[index].u;
		davert[index].TextUV.y=dauvs[index].v;
		davert[index].Norm.x=danormals[index].x;
		davert[index].Norm.y=danormals[index].y;
		davert[index].Norm.z=danormals[index].z;
	}

	if(dacolors.size()!=0)
	{
		for(index=0;index<newmeshpart->numvert;index++)
		{
			davert[index].Color.x=dacolors[index].r;
			davert[index].Color.y=dacolors[index].g;
			davert[index].Color.z=dacolors[index].b;
			davert[index].Color.w=dacolors[index].a;
		}
	}
	else
	{
		for(index=0;index<newmeshpart->numvert;index++)
		{
			davert[index].Color.x=1.0f;
			davert[index].Color.y=1.0f;
			davert[index].Color.z=1.0f;
			davert[index].Color.w=1.0f;
		}
	}

	//Load the texture
	vector<NiPropertyRef> daprops=trishapenode->GetProperties();
	bool foundtext=false;
	for(index=0;index<daprops.size();index++)
	{
		if(daprops[index]->IsSameType(NiTexturingProperty::TYPE))
		{
			NiTexturingPropertyRef datextprop=DynamicCast<NiTexturingProperty>(daprops[index]);
			if(datextprop==NULL)
			{
				output_text(L"Failed NiTexturingProperty Cast!\r\n");
				delete newmeshpart;
				delete [] davert;
				delete [] daind;
				return NULL;
			}
			foundtext=true;
			TexDesc datextdesc=datextprop->GetTexture(0);
			newmeshpart->datext=pTextureManager->Load_Direct((char *)datextdesc.source->GetTextureFileName().c_str());
			break;
		}
	}

	if(foundtext==false) output_text_hex(L"Couldn't find texture:",formID);

	newmeshpart->pIndiceBuf=pDXManager->CreateIndiceBuffer((unsigned char *)daind,newmeshpart->numind,sizeof(unsigned short));
	newmeshpart->pVertexBuf=pDXManager->CreateVertexBuffer((unsigned char *)davert,newmeshpart->numvert,sizeof(MeshVertex));

	delete [] daind;
	delete [] davert;
	
	//Ok now let's add it
	listdata.push_back(newmeshpart);

	return newmeshpart;
}

unsigned long CMesh::Load_Mesh(unsigned long want_formID)
{
	unsigned long index;
	CStat_Record *dastat;
	MeshData *dameshdata;
	char tmppath[MAX_PATH];

	dastat=pTreeManager->GetStat(want_formID);
	if(dastat==NULL) return 1;

	formID=want_formID;

	strcpy_s(tmppath,MAX_PATH,"data\\meshes\\");
	strcat_s(tmppath,MAX_PATH,dastat->Get_Filename());

	//output_text_char(L"Filename: ",tmppath);
	
	NiObjectRef root=ReadNifTree(tmppath);

	NiNodeRef node = DynamicCast<NiNode>(root);
	if(node==NULL) return 2;

	vector<NiAVObjectRef> refs=node->GetChildren();
	for(index=0;index<refs.size();index++)
	{
		if(refs[index]->IsSameType(NiTriStrips::TYPE))
		{
			NiTriStripsRef datristrip=DynamicCast<NiTriStrips>(refs[index]);
			//Add a tri strip buffer to the mesh
			dameshdata=AddTriStrips(datristrip);
			if(dameshdata==NULL)
			{
				output_text(L"Error AddTriStrips\r\n");
				return 3;
			}
			Vector3 translation=datristrip->GetLocalTranslation();
			dameshdata->local_translation[0]=translation.x;
			dameshdata->local_translation[1]=translation.y;
			dameshdata->local_translation[2]=translation.z;
			dameshdata->tristrip=1;
		}
		else
		{
			if(refs[index]->IsSameType(NiTriShape::TYPE))
			{
				NiTriShapeRef datrishape=DynamicCast<NiTriShape>(refs[index]);
				//Add a tri shape to the mesh
				dameshdata=AddTriShape(datrishape);
				if(dameshdata==NULL)
				{
					output_text(L"Error AddTriShape\r\n");
					return 3;
				}
				Vector3 translation=datrishape->GetLocalTranslation();
				dameshdata->local_translation[0]=translation.x;
				dameshdata->local_translation[1]=translation.y;
				dameshdata->local_translation[2]=translation.z;
				dameshdata->tristrip=0;
			}
		}
	}

	//Load the collision data
	/*NiCollisionObjectRef dacol=node->GetCollisionObject();
	if(dacol->IsSameType(bhkCollisionObject::TYPE))
	{
		bhkCollisionObjectRef bhkCol=DynamicCast<bhkCollisionObject>(dacol);
		NiObjectRef daobj=bhkCol->GetBody();
		if(daobj->IsSameType(bhkRigidBody::TYPE))
		{
			bhkRigidBody
		}
		
	}*/

	return 0;
}

unsigned long CMesh::Get_FormID()
{
	return formID;
}

void CMesh::Draw(float *position,float *rotation,float scale)
{
	unsigned long index;
	float final_position[3];

	for(index=0;index<listdata.size();index++)
	{
		final_position[0]=position[0]+listdata[index]->local_translation[0];
		final_position[1]=position[1]+listdata[index]->local_translation[1];
		final_position[2]=position[2]+listdata[index]->local_translation[2];
		pDXManager->SetSimpleMeshTexture(listdata[index]->datext->Get_Texture());
		if(listdata[index]->tristrip==1)
		{
			pDXManager->DrawMeshStrip(listdata[index]->pVertexBuf,listdata[index]->pIndiceBuf,listdata[index]->numind,sizeof(MeshVertex),final_position,rotation,scale);
		}
		else
		{
			pDXManager->DrawMesh(listdata[index]->pVertexBuf,listdata[index]->pIndiceBuf,listdata[index]->numind,sizeof(MeshVertex),final_position,rotation,scale);
		}

	}
}

CMeshManager::CMeshManager()
{
}

CMeshManager::~CMeshManager()
{
}

unsigned long numobjectloaded=0;

CMesh *CMeshManager::Load_STAT(unsigned long want_formID)
{
	unsigned long index;
	CMesh *damesh;

	//output_text(L"Load stat called...\r\n");
	for(index=0;index<listmeshes.size();index++)
	{
		if(listmeshes[index]->Get_FormID()==want_formID) return listmeshes[index];
	}

	damesh=new CMesh();

	unsigned long result;
	//output_text_hex(L"Looking for:",want_formID);
	try {
		if(result=damesh->Load_Mesh(want_formID)!=0)
		{
			//output_text_value(L"Error from Load Mesh:",result);
			delete damesh;
			return NULL;
		}
	}
	catch(exception &e)
	{
		output_text_char(L"C++ exception occured:",(char *)e.what());
		output_text_hex(L"On CMesh::Load_Mesh with formID:",want_formID);
		delete damesh;
		return NULL;
	}


	//numobjectloaded++;
	//if(numobjectloaded%10==0) output_text_value(L"Number of object loaded:",numobjectloaded);

	return damesh;
}

