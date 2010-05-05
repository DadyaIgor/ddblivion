#include "stdafx.h"
#include "Global.h"
#include "CTreeManager.h"
#include "CGlobal.h"
#include "CTextureManager.h"
#include "CMeshManager.h"
#include "CLandManager.h"

ID3D10Buffer *pLandIndices;

CLand::CLand()
{
	VertexBufs[0]=NULL;
	VertexBufs[1]=NULL;
	VertexBufs[2]=NULL;
	VertexBufs[3]=NULL;
	x=0;
	y=0;
	formID=0;
	close_meshes_loaded=false;
	distant_meshes_loaded=false;
	crit_mesh_loaded=NULL;

	crit_mesh_loaded=new CRITICAL_SECTION;
	if(!InitializeCriticalSectionAndSpinCount(crit_mesh_loaded,4000))
	{
		CLEAN_DELETE(crit_mesh_loaded);
		throw L"Couldn't init crit_mesh_loaded...";
	}
}

CLand::~CLand()
{
	if(VertexBufs[0]) delete [] VertexBufs[0];
	if(VertexBufs[1]) delete [] VertexBufs[1];
	if(VertexBufs[2]) delete [] VertexBufs[2];
	if(VertexBufs[3]) delete [] VertexBufs[3];
	if(crit_mesh_loaded)
	{
		DeleteCriticalSection(crit_mesh_loaded);
		CLEAN_DELETE(crit_mesh_loaded);
	}
}

void CLand::Get_XY(long &_x,long &_y)
{
	_x=x;
	_y=y;
}

void CLand::SetNormals(char *data)
{
	unsigned long index,index2;

	for(index=0;index<17;index++)
	{
		for(index2=0;index2<17;index2++)
		{
			VertexBufs[0][(index*17)+index2].Norm.x=(data[(((index*33)+index2)*3)]/255.0f);
			VertexBufs[0][(index*17)+index2].Norm.y=(data[(((index*33)+index2)*3)+1]/255.0f);
			VertexBufs[0][(index*17)+index2].Norm.z=(data[(((index*33)+index2)*3)+2]/255.0f);
		}
	}

	for(index=0;index<17;index++)
	{
		for(index2=16;index2<33;index2++)
		{
			VertexBufs[1][(index*17)+(index2-16)].Norm.x=(data[(((index*33)+index2)*3)]/255.0f);
			VertexBufs[1][(index*17)+(index2-16)].Norm.y=(data[(((index*33)+index2)*3)+1]/255.0f);
			VertexBufs[1][(index*17)+(index2-16)].Norm.z=(data[(((index*33)+index2)*3)+2]/255.0f);
		}
	}

	for(index=16;index<33;index++)
	{
		for(index2=0;index2<17;index2++)
		{
			VertexBufs[2][((index-16)*17)+index2].Norm.x=(data[(((index*33)+index2)*3)]/255.0f);
			VertexBufs[2][((index-16)*17)+index2].Norm.y=(data[(((index*33)+index2)*3)+1]/255.0f);
			VertexBufs[2][((index-16)*17)+index2].Norm.z=(data[(((index*33)+index2)*3)+2]/255.0f);
		}
	}

	for(index=16;index<33;index++)
	{
		for(index2=16;index2<33;index2++)
		{
			VertexBufs[3][((index-16)*17)+(index2-16)].Norm.x=(data[(((index*33)+index2)*3)]/255.0f);
			VertexBufs[3][((index-16)*17)+(index2-16)].Norm.y=(data[(((index*33)+index2)*3)+1]/255.0f);
			VertexBufs[3][((index-16)*17)+(index2-16)].Norm.z=(data[(((index*33)+index2)*3)+2]/255.0f);
		}
	}
}

void CLand::SetPositionsAndHeights(float offset,char *data)
{
	unsigned long index,index2;

	unsigned long colIndex,lineIndex,tempIndex;

	tabHeight[0]=(offset*SCALEHEIGHT);

	for(lineIndex=0;lineIndex<33;lineIndex++)
	{
		for(colIndex=0;colIndex<33;colIndex++)
		{
			if(colIndex==0) //First item on a line
			{
				if(lineIndex!=0) tabHeight[lineIndex*33]+=((data[(lineIndex*33)])*SCALEHEIGHT);
				//Propagate north
				for(tempIndex=(lineIndex+1);tempIndex<33;tempIndex++)
				{
					tabHeight[tempIndex*33]=tabHeight[lineIndex*33];
				}
				//Propagate east
				for(tempIndex=1;tempIndex<33;tempIndex++)
				{
					tabHeight[(lineIndex*33)+tempIndex]=tabHeight[lineIndex*33];
				}
			}
			else
			{
				//Propagate east
				tabHeight[(lineIndex*33)+colIndex]+=((data[(lineIndex*33)+colIndex])*SCALEHEIGHT);
				for(tempIndex=(colIndex+1);tempIndex<33;tempIndex++)
				{
					tabHeight[(lineIndex*33)+tempIndex]=tabHeight[(lineIndex*33)+colIndex];
				}
			}
		}
	}

	//NEW:Generate 4 VertexBuf
	for(index=0;index<17;index++)
	{
		for(index2=0;index2<17;index2++)
		{
			VertexBufs[0][(index*17)+index2].Pos.x=(x*CELLSIZE)+index2*SCALE;
			VertexBufs[0][(index*17)+index2].Pos.y=(y*CELLSIZE)+index*SCALE;
			VertexBufs[0][(index*17)+index2].Pos.z=tabHeight[(index*33)+index2];
		}
	}

	for(index=0;index<17;index++)
	{
		for(index2=16;index2<33;index2++)
		{
			VertexBufs[1][(index*17)+(index2-16)].Pos.x=(x*CELLSIZE)+index2*SCALE;
			VertexBufs[1][(index*17)+(index2-16)].Pos.y=(y*CELLSIZE)+index*SCALE;
			VertexBufs[1][(index*17)+(index2-16)].Pos.z=tabHeight[(index*33)+index2];
		}
	}

	for(index=16;index<33;index++)
	{
		for(index2=0;index2<17;index2++)
		{
			VertexBufs[2][((index-16)*17)+index2].Pos.x=(x*CELLSIZE)+index2*SCALE;
			VertexBufs[2][((index-16)*17)+index2].Pos.y=(y*CELLSIZE)+index*SCALE;
			VertexBufs[2][((index-16)*17)+index2].Pos.z=tabHeight[(index*33)+index2];
		}
	}

	for(index=16;index<33;index++)
	{
		for(index2=16;index2<33;index2++)
		{
			VertexBufs[3][((index-16)*17)+(index2-16)].Pos.x=(x*CELLSIZE)+index2*SCALE;
			VertexBufs[3][((index-16)*17)+(index2-16)].Pos.y=(y*CELLSIZE)+index*SCALE;
			VertexBufs[3][((index-16)*17)+(index2-16)].Pos.z=tabHeight[(index*33)+index2];
		}
	}

	//Set the 4 vertices for view frustrum culling
			VertexBufs[0][(index*17)+index2].Pos.x=(x*CELLSIZE)+index2*SCALE;
			VertexBufs[0][(index*17)+index2].Pos.y=(y*CELLSIZE)+index*SCALE;
			VertexBufs[0][(index*17)+index2].Pos.z=tabHeight[(index*33)+index2];

	border_points[0][0]=(x*CELLSIZE);
	border_points[0][1]=(y*CELLSIZE);
	border_points[0][2]=tabHeight[0];

	border_points[1][0]=(x*CELLSIZE)+(32.0f*SCALE);
	border_points[1][1]=(y*CELLSIZE);
	border_points[1][2]=tabHeight[32];

	border_points[2][0]=(x*CELLSIZE);
	border_points[2][1]=(y*CELLSIZE)+(32.0f*SCALE);
	border_points[2][2]=tabHeight[32*33];

	border_points[3][0]=(x*CELLSIZE)+(32.0f*SCALE);
	border_points[3][1]=(y*CELLSIZE)+(32.0f*SCALE);
	border_points[3][2]=tabHeight[(32*33)+32];
}

void CLand::SetColors(unsigned char *cdata)
{
	unsigned long index,index2;

	//NEW:Generate 4 VertexBuf
	for(index=0;index<17;index++)
	{
		for(index2=0;index2<17;index2++)
		{
			VertexBufs[0][(index*17)+index2].Color.x=(cdata[(((index*33)+index2)*3)]/255.0f);
			VertexBufs[0][(index*17)+index2].Color.y=(cdata[(((index*33)+index2)*3)+1]/255.0f);
			VertexBufs[0][(index*17)+index2].Color.z=(cdata[(((index*33)+index2)*3)+2]/255.0f);
			VertexBufs[0][(index*17)+index2].Color.w=1.0f;
		}
	}

	for(index=0;index<17;index++)
	{
		for(index2=16;index2<33;index2++)
		{
			VertexBufs[1][(index*17)+(index2-16)].Color.x=(cdata[(((index*33)+index2)*3)]/255.0f);
			VertexBufs[1][(index*17)+(index2-16)].Color.y=(cdata[(((index*33)+index2)*3)+1]/255.0f);
			VertexBufs[1][(index*17)+(index2-16)].Color.z=(cdata[(((index*33)+index2)*3)+2]/255.0f);
			VertexBufs[1][(index*17)+(index2-16)].Color.w=1.0f;
		}
	}

	for(index=16;index<33;index++)
	{
		for(index2=0;index2<17;index2++)
		{
			VertexBufs[2][((index-16)*17)+index2].Color.x=(cdata[(((index*33)+index2)*3)]/255.0f);
			VertexBufs[2][((index-16)*17)+index2].Color.y=(cdata[(((index*33)+index2)*3)+1]/255.0f);
			VertexBufs[2][((index-16)*17)+index2].Color.z=(cdata[(((index*33)+index2)*3)+2]/255.0f);
			VertexBufs[2][((index-16)*17)+index2].Color.w=1.0f;
		}
	}

	for(index=16;index<33;index++)
	{
		for(index2=16;index2<33;index2++)
		{
			VertexBufs[3][((index-16)*17)+(index2-16)].Color.x=(cdata[(((index*33)+index2)*3)]/255.0f);
			VertexBufs[3][((index-16)*17)+(index2-16)].Color.y=(cdata[(((index*33)+index2)*3)+1]/255.0f);
			VertexBufs[3][((index-16)*17)+(index2-16)].Color.z=(cdata[(((index*33)+index2)*3)+2]/255.0f);
			VertexBufs[3][((index-16)*17)+(index2-16)].Color.w=1.0f;
		}
	}

}

void CLand::SetBaseTexture(unsigned long textID,unsigned char quadrant)
{
	unsigned long index,index2;

	pTextureList[quadrant][0]=pTextureManager->Load_LTEX(textID);
	if(pTextureList[quadrant][0]==NULL)
	{
		output_text_value(L"Error loading Base Texture in CELL:",formID);
	}
	//Set at 0 every weight for this quadrant
	for(index=0;index<(17*17);index++)
	{
		for(index2=0;index2<9;index2++)
		{
			VertexBufs[quadrant][index].weights[index2]=0.0f;
		}
	}

	//Calculate UVs
	for(index=0;index<17;index++)
	{
		for(index2=0;index2<17;index2++)
		{
			VertexBufs[quadrant][(index*17)+index2].TextUV.x=(float)(index2*SCALETEXT);
			VertexBufs[quadrant][(index*17)+index2].TextUV.y=(float)(index*SCALETEXT);
		}
	}

	//Ensure that there is no loose textures
	for(index2=1;index2<9;index2++)
	{
		pTextureList[quadrant][index2]=pTextureList[quadrant][0];
	}
}

void CLand::CalculateBaseOpacities()
{
	float result;
	unsigned long index,index2,index3;

	for(index=0;index<4;index++)
	{
		for(index2=0;index2<(17*17);index2++)
		{
			result=0.0f;
			for(index3=1;index3<9;index3++)
			{
				result+=VertexBufs[index][index2].weights[index3];
			}
			if(result<1.0f) VertexBufs[index][index2].weights[0]=(1.0f-result);
			else VertexBufs[index][index2].weights[0]=0.0f;
		}
	}
}


void CLand::SetAlphaTexture(unsigned long textID,unsigned char quadrant,unsigned short layer)
{
	pTextureList[quadrant][(layer+1)]=pTextureManager->Load_LTEX(textID);
	if(pTextureList[quadrant][(layer+1)]==NULL)
	{
		output_text_value(L"Error loading Alpha Texture in CELL:",formID);
	}
}

void CLand::ProcessVTXT(unsigned long numentries,float *opacities,unsigned short *locations,unsigned short layer,unsigned char quadrant)
{
	unsigned long index;

	for(index=0;index<numentries;index++)
	{
		VertexBufs[quadrant][locations[index]].weights[(layer+1)]=opacities[index];
	}
}

void CLand::Load(CCell_Record *cell_rec)
{
	unsigned long index,index2,numSub;

	CLand_Record *the_land;
	CSubRecord_Entry *curEntry;
	CSubVNML_Entry *vnmlEntry;
	CSubVHGT_Entry *vhgtEntry;
	CSubVCLR_Entry *vclrEntry;
	CSubBTXT_Entry *btxtEntry;
	CSubATXT_Entry *atxtEntry;
	CSubVTXT_Entry *vtxtEntry;

	char *height_data;
	float height_offset;
	
	unsigned char *color_data;
	
	unsigned char quadrant;
	unsigned long textID;
	unsigned short layer;

	unsigned short *locations;
	float *opacities;
	unsigned long num_data;

	unsigned long toskip=0;

	the_cell=cell_rec;

	the_cell->Get_XY(x,y);
	formID=the_cell->Get_FormID();

	//Allocate memory for the vertex buffer;

	for(index=0;index<4;index++)
	{
		VertexBufs[index]=new VertexLand[(17*17)]; // 289
	}

	CTexture *defaulttext;
	defaulttext=pTextureManager->GetDefaultTexture();

	for(index=0;index<4;index++)
	{
		for(index2=0;index2<9;index2++)
		{
			pTextureList[index][index2]=defaulttext;
		}
	}


	//VertexBuf=new VertexLand[1089];

	the_land=the_cell->GetAssociatedLand();
	if(!the_land) throw L"No associated land!";

	//output_text(L"going for land...\r\n");

	numSub=the_land->GetNumSubRecords();
	for(index=0;index<numSub;index++)
	{
		curEntry=the_land->GetSubRecord(index);
		switch(curEntry->Get_SubRecordType())
		{
		case VNML:
			//output_text(L"VNML\r\n");
			vnmlEntry=(CSubVNML_Entry *)curEntry;
			SetNormals(vnmlEntry->Get_Normals());
			break;
		case VHGT:
			//output_text(L"VHGT\r\n");
			vhgtEntry=(CSubVHGT_Entry *)curEntry;
			height_data=vhgtEntry->Get_Height(height_offset);
			SetPositionsAndHeights(height_offset,height_data);
			break;
		case VCLR:
			//output_text(L"VCLR\r\n");
			vclrEntry=(CSubVCLR_Entry *)curEntry;
			color_data=vclrEntry->Get_Colors();
			SetColors(color_data);
			break;
		case BTXT:
			//output_text(L"BTXT\r\n");
			btxtEntry=(CSubBTXT_Entry *)curEntry;
			textID=btxtEntry->Get_TextureFormID(quadrant);
			SetBaseTexture(textID,quadrant);
			break;
		case ATXT:
			//output_text(L"ATXT\r\n");
			atxtEntry=(CSubATXT_Entry *)curEntry;
			textID=atxtEntry->Get_TextureFormID(quadrant,layer);
			if(textID==0)
			{
				toskip=1;
				break;
			}
			SetAlphaTexture(textID,quadrant,layer);
			break;
		case VTXT:
			//output_text(L"VTXT\r\n");
			if(toskip==0)
			{
				vtxtEntry=(CSubVTXT_Entry *)curEntry;
				num_data=vtxtEntry->Get_Opacities(locations,opacities);
				ProcessVTXT(num_data,opacities,locations,layer,quadrant);
			}
			else
			{
				toskip=0;
			}
			break;
		default:
			break;
		}
	}

	//output_text(L"Calculate opacities...\r\n");
	CalculateBaseOpacities();
	
	//output_text(L"Create buffers...\r\n");
	//Create the buffers for the GC
	for(index=0;index<4;index++)
	{
		pFinalBufs[index]=pDXManager->CreateVertexBuffer((unsigned char *)VertexBufs[index],17*17,sizeof(VertexLand));
	}

	//Delete the vertex bufs(only the final bufs are necessary)
	for(index=0;index<4;index++)
	{
		delete [] VertexBufs[index];
	}
	
	//Create the rigid body for Havok
	landBody=pHavokManager->CreateLandscapeRigid(tabHeight,(x*(CELLSIZE/SCALE_HAVOK)),(y*(CELLSIZE/SCALE_HAVOK)));
}

void CLand::LoadDistantMeshes()
{
	bool isLoaded;

	EnterCriticalSection(crit_mesh_loaded);
	isLoaded=distant_meshes_loaded;
	LeaveCriticalSection(crit_mesh_loaded);

	if(isLoaded==true) return;

	CGrup_Entry *curGrup;
	CRecord_Entry *curRec;
	CRefr_Record *curRefr;
	GrupHeader dagrup;
	unsigned long daformid,index;
	CMesh *damesh;
	Mesh_Refr *darefr;

	//load the far meshes
	curGrup=the_cell->GetAssociatedGrup();
	if(curGrup!=NULL)
	{
		dagrup.groupType=CELL_VISI_DIST;
		dagrup.label.formID=curGrup->Get_FormID();
		curGrup=curGrup->Find_Grup(dagrup);
		if(curGrup!=NULL)
		{
			for(index=0;index<curGrup->GetNumRecords();index++)
			{
				curRec=curGrup->GetRecord(index);
				if(curRec->Get_RecordType()==REFR)
				{
					curRefr=(CRefr_Record *)curRec;
					daformid=curRefr->Get_Reference();
					//output_text_hex(L"Current reference points to:",daformid);
					if(daformid!=0)
					{
						damesh=pMeshManager->Load_STAT(daformid);
						if(damesh!=NULL)
						{
							darefr=new Mesh_Refr;
							darefr->damesh=damesh;
							darefr->pos_and_rot=curRefr->Get_PosAndRot();
							darefr->scale=curRefr->Get_Scale();
							listdistantmeshes.push_back(darefr);
						}
					}
				}
			}
		}
	}
	EnterCriticalSection(crit_mesh_loaded);
	distant_meshes_loaded=true;
	LeaveCriticalSection(crit_mesh_loaded);
}

void CLand::LoadCloseMeshes()
{
	bool isLoaded;

	EnterCriticalSection(crit_mesh_loaded);
	isLoaded=close_meshes_loaded;
	LeaveCriticalSection(crit_mesh_loaded);

	if(isLoaded==true) return;

	CGrup_Entry *curGrup;
	CRecord_Entry *curRec;
	CRefr_Record *curRefr;
	GrupHeader dagrup;
	unsigned long daformid,index;
	CMesh *damesh;
	Mesh_Refr *darefr;

	//load the close meshes
	curGrup=the_cell->GetAssociatedGrup();
	if(curGrup!=NULL)
	{
		dagrup.groupType=CELL_TEMP_CHILD;
		dagrup.label.formID=curGrup->Get_FormID();
		curGrup=curGrup->Find_Grup(dagrup);
		if(curGrup!=NULL)
		{
			for(index=0;index<curGrup->GetNumRecords();index++)
			{
				curRec=curGrup->GetRecord(index);
				if(curRec->Get_RecordType()==REFR)
				{
					curRefr=(CRefr_Record *)curRec;
					daformid=curRefr->Get_Reference();
					//output_text_hex(L"Current reference points to:",daformid);
					if(daformid!=0)
					{
						damesh=pMeshManager->Load_STAT(daformid);
						if(damesh!=NULL)
						{
							darefr=new Mesh_Refr;
							darefr->damesh=damesh;
							darefr->pos_and_rot=curRefr->Get_PosAndRot();
							darefr->scale=curRefr->Get_Scale();
							listclosemeshes.push_back(darefr);
						}
					}
				}
			}
		}
	}
	EnterCriticalSection(crit_mesh_loaded);
	close_meshes_loaded=true;
	LeaveCriticalSection(crit_mesh_loaded);
}

void CLand::ActivatePhysics()
{
	if(landBody->getReferenceCount()==1)
	{
		pHavokManager->AddRigidBody(landBody);
	}
}

void CLand::Draw()
{
	unsigned long index,index2;

	for(index=0;index<4;index++)
	{
		for(index2=0;index2<9;index2++)
		{
			pDXManager->SetLandscapeTexture(pTextureList[index][index2]->Get_Texture(),index2);
		}
		pDXManager->DrawLandscapeQuadrant(pFinalBufs[index],pLandIndices,(((17*2)*(17-1))+(17-2)),sizeof(VertexLand));
	}
}

void CLand::DrawCloseMeshes()
{
	unsigned long index;
	bool isLoaded;

	EnterCriticalSection(crit_mesh_loaded);
	isLoaded=close_meshes_loaded;
	LeaveCriticalSection(crit_mesh_loaded);

	if(isLoaded==false) return;

	for(index=0;index<listclosemeshes.size();index++)
	{
		listclosemeshes[index]->damesh->Draw(&listclosemeshes[index]->pos_and_rot[0],&listclosemeshes[index]->pos_and_rot[3],listclosemeshes[index]->scale);
	}
}

void CLand::DrawDistantMeshes()
{
	unsigned long index;
	bool isLoaded;

	EnterCriticalSection(crit_mesh_loaded);
	isLoaded=distant_meshes_loaded;
	LeaveCriticalSection(crit_mesh_loaded);

	if(isLoaded==false) return;


	for(index=0;index<listdistantmeshes.size();index++)
	{
		listdistantmeshes[index]->damesh->Draw(&listdistantmeshes[index]->pos_and_rot[0],&listdistantmeshes[index]->pos_and_rot[3],listdistantmeshes[index]->scale);
	}
}

void CLand::CheckAgainstFrustrum()
{
	unsigned long index;

	for(index=0;index<4;index++)
	{
		if(pDXManager->IsVertexVisible(&border_points[index][0])==true)
		{
			frustrum_visible=true;
			return;
		}
	}

	frustrum_visible=false;
}

bool CLand::IsVisible()
{
	return frustrum_visible;
}

CLandManager::CLandManager()
{
	crit_ListLands=NULL;
	StreamLoaderHandle=NULL;
}

CLandManager::~CLandManager()
{
}

void CLandManager::Init()
{
	short tempx,tempz;
	unsigned short *indices,*findices;
	unsigned long index;

	//Generate the indice buffer for landscape(once only)
	indices=new unsigned short[((17*2)*(17-1))+(17-2)];
	findices=new unsigned short[((17*2)*(17-1))+(17-2)];

	index=0;
	for(tempz=0;tempz<(17-1);tempz++)
	{
		if((tempz%2)==0)
		{
			for(tempx=0;tempx<17;tempx++)
			{
				indices[index++]=tempx+(tempz*17);
				indices[index++]=tempx+(tempz*17)+17;
			}
			//Degenerate vertex
			if(tempz!=(17-2))
			{
				indices[index++]=((tempz+1)*17)+(tempx-1);
			}
		}
		else
		{
			for(tempx=(17-1);tempx>=0;tempx--)
			{
				indices[index++]=tempx+(tempz*17);
				indices[index++]=tempx+(tempz*17)+17;
			}
			//Degenerate vertex
			if(tempz!=(17-2))
			{
				indices[index++]=(tempx+1)+((tempz+1)*17);
			}
		}
	}

	for(index=0;index<(((17*2)*(17-1))+(17-2));index++)
	{
		findices[index]=indices[(((17*2)*(17-1))+(17-2))-(index+1)];
	}

	pLandIndices=pDXManager->CreateIndiceBuffer((unsigned char *)findices,(((17*2)*(17-1))+(17-2)),sizeof(unsigned short));
	
	delete [] indices;
	delete [] findices;
}

DWORD WINAPI StreamingThreadFunc(LPVOID lpParam)
{
	float playerpos[3];
	hkMemoryRouter StreamMemoryRouter;
	long curcellx,curcelly,oldcellx,oldcelly;
	CLandManager *landManager=(CLandManager *)lpParam;

	pHavokManager->CreateThreadMemoryRouter(StreamMemoryRouter);

	pGlobal->GetPlayerPosition(playerpos);
	oldcellx=(long)(playerpos[0]/CELLSIZE);
	oldcelly=(long)(playerpos[1]/CELLSIZE);

	while(1)
	{
		pGlobal->GetPlayerPosition(playerpos);
		curcellx=(long)(playerpos[0]/CELLSIZE);
		curcelly=(long)(playerpos[1]/CELLSIZE);
		if((curcellx!=oldcellx)||(curcelly!=oldcelly))
		{
			landManager->UpdateInternalView(curcellx,curcelly);
		}
		else
		{
			Sleep(500);
		}
	}
}


void CLandManager::InitWorld()
{
	CCell_Record *cur_cell;
	CLand *cur_land;
	float playerpos[3];
	long cellx,celly;

	for(celly=0;celly<200;celly++)
	{
		for(cellx=0;cellx<200;cellx++)
		{
			listLands[celly][cellx]=NULL;
		}
	}
	
	//First determine the cell where the player is
	pGlobal->GetPlayerPosition(playerpos);
	curcellx=(long)(playerpos[0]/CELLSIZE);
	curcelly=(long)(playerpos[1]/CELLSIZE);

	//Now let's load the whole block around it(current cell included)
	for(celly=-RENDERCELLRADIUS;celly<(RENDERCELLRADIUS+1);celly++)
	{
		for(cellx=-RENDERCELLRADIUS;cellx<(RENDERCELLRADIUS+1);cellx++)
		{
			cur_cell=pTreeManager->GetCurWrldCell(curcellx+cellx,curcelly+celly);
			if(cur_cell)
			{
				if(cur_cell->GetAssociatedLand())
				{
					cur_land=new CLand();
					listLands[LANDOFFSET_Y+(curcelly+celly)][LANDOFFSET_X+(curcellx+cellx)]=cur_land;
					cur_land->Load(cur_cell);
					cur_land->LoadDistantMeshes();
				}
			}
		}
	}
	
	for(celly=-HAVOKACTIVECELLS;celly<(HAVOKACTIVECELLS+1);celly++)
	{
		for(cellx=-HAVOKACTIVECELLS;cellx<(HAVOKACTIVECELLS+1);cellx++)
		{
			cur_land=listLands[LANDOFFSET_Y+(curcelly+celly)][LANDOFFSET_X+(curcellx+cellx)];
			if(cur_land)
			{
				cur_land->ActivatePhysics();
				cur_land->LoadCloseMeshes();
			}
		}
	}

	//Create the quadtree

	//Start by creating a critical section for the list of lands
	crit_ListLands=new CRITICAL_SECTION;
	if(!InitializeCriticalSectionAndSpinCount(crit_ListLands,4000))
	{
		CLEAN_DELETE(crit_ListLands);
		throw L"Failed to initialize critical section for ListLands...";
	}
	
	//Copy what is Loaded for the streamer
	memcpy(internalLands,listLands,200*200*sizeof(CLand *));

	//Create the thread that will handle the loading from now on...
	StreamLoaderHandle=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)StreamingThreadFunc,(LPVOID)this,0,&Thread_StreamLoader_ID);
	if(StreamLoaderHandle==NULL) throw L"Failed to create StreamingThreadFunc thread...";
}

void CLandManager::CreateQuadTree(long pos[2], unsigned long size)
{

}

void CLandManager::UpdateInternalView(long ncellx,long ncelly)
{
	CLand *cur_land;
	CCell_Record *cur_cell;
	long cellx,celly;

	for(celly=-(RENDERCELLRADIUS+2);celly<(RENDERCELLRADIUS+3);celly++)
	{
		for(cellx=-(RENDERCELLRADIUS+2);cellx<(RENDERCELLRADIUS+3);cellx++)
		{
			cur_land=internalLands[LANDOFFSET_Y+(ncelly+celly)][LANDOFFSET_X+(ncellx+cellx)];
			if(cur_land==NULL)
			{
				cur_cell=pTreeManager->GetCurWrldCell(ncellx+cellx,ncelly+celly);
				if(cur_cell)
				{
					if(cur_cell->GetAssociatedLand())
					{
						cur_land=new CLand();
						internalLands[LANDOFFSET_Y+(ncelly+celly)][LANDOFFSET_X+(ncellx+cellx)]=cur_land;
						cur_land->Load(cur_cell);
						cur_land->LoadDistantMeshes();
					}
				}
			}
		}
	}

	for(celly=-HAVOKACTIVECELLS;celly<(HAVOKACTIVECELLS+1);celly++)
	{
		for(cellx=-HAVOKACTIVECELLS;cellx<(HAVOKACTIVECELLS+1);cellx++)
		{
			cur_land=internalLands[LANDOFFSET_Y+(curcelly+celly)][LANDOFFSET_X+(curcellx+cellx)];
			if(cur_land)
			{
				cur_land->ActivatePhysics();
				cur_land->LoadCloseMeshes();
			}
		}
	}

	//Copy the updated stuff
	EnterCriticalSection(crit_ListLands);

	memcpy(listLands,internalLands,200*200*sizeof(CLand *));

	LeaveCriticalSection(crit_ListLands);

}

void CLandManager::Draw()
{
	long cellx,celly;
	CLand *cur_land;
	float playerpos[3];

	pGlobal->GetPlayerPosition(playerpos);
	curcellx=(long)(playerpos[0]/CELLSIZE);
	curcelly=(long)(playerpos[1]/CELLSIZE);

	EnterCriticalSection(crit_ListLands);

	for(celly=-RENDERCELLRADIUS;celly<(RENDERCELLRADIUS+1);celly++)
	{
		for(cellx=-RENDERCELLRADIUS;cellx<(RENDERCELLRADIUS+1);cellx++)
		{
			cur_land=listLands[LANDOFFSET_Y+(curcelly+celly)][LANDOFFSET_X+(curcellx+cellx)];
			if(cur_land)
			{
				cur_land->CheckAgainstFrustrum();
				if(cur_land->IsVisible()==true||((celly==0)&&(cellx==0)))
				{
					cur_land->Draw();
					cur_land->DrawDistantMeshes();
				}
			}
		}
	}

	for(celly=-RENDERCLOSEMESHRADIUS;celly<(RENDERCLOSEMESHRADIUS+1);celly++)
	{
		for(cellx=-RENDERCLOSEMESHRADIUS;cellx<(RENDERCLOSEMESHRADIUS+1);cellx++)
		{
			cur_land=listLands[LANDOFFSET_Y+(curcelly+celly)][LANDOFFSET_X+(curcellx+cellx)];
			if(cur_land)
			{
				if(cur_land->IsVisible()==true) cur_land->DrawCloseMeshes();
			}
		}
	}

	LeaveCriticalSection(crit_ListLands);
}

