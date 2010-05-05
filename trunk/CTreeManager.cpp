#include "stdafx.h"
#include "Global.h"
#include "CTreeManager.h"
#include "CModManager.h"
#include <ZLib.h>

#define NUM_TAGS 64
char *Tag_Records[NUM_TAGS] = {
	"ACHR","ACRE","ACTI","ALCH","AMMO","ANIO","APPA","ARMO","BOOK","BSGN","CELL","CLAS","CLMT","CLOT","CONT","CREA","CSTY","DIAL","DOOR","EFSH","ENCH",
	"EYES","FACT","FLOR","FURN","GLOB","GMST","GRAS","HAIR","IDLE","INFO","INGR","KEYM","LAND","LIGH","LSCR","LTEX","LVLC","LVLI","LVSP","MGEF","MISC",
	"NPC_","PACK","PGRD","QUST","RACE","REFR","REGN","ROAD","SBSP","SCPT","SGST","SKIL","SLGM","SOUN","SPEL","STAT","TES4","TREE","WATR","WEAP","WRLD","WTHR"
};

#define NUM_SUBTAGS 21
char *SubTag_Records[NUM_SUBTAGS] = { "XCLC","VHGT","EDID","XOWN","XGLB","XCCM","XCWT","XCLR","WNAM","CNAM",
									  "NAM2","ATXT","BTXT","ICON","VNML","VCLR","VTXT","MODL","NAME","DATA",
									  "XSCL" };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RECORD ENTRY//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRecord_Entry::CRecord_Entry(unsigned long _type,RecordHeader daheader,CModule *cur_mod)
{
	record_type=_type;
	memcpy(&thisRecordHeader,&daheader,sizeof(RecordHeader));
	thisRecordHeader.formid=cur_mod->TranslateFormID(thisRecordHeader.formid);
	listsubrecords.clear();
}

CRecord_Entry::~CRecord_Entry()
{
	unsigned long index;
	for(index=0;index<listsubrecords.size();index++)
	{
		delete listsubrecords[index];
	}
	listsubrecords.clear();
}

unsigned long CRecord_Entry::GetNumSubRecords()
{
	return (unsigned long)listsubrecords.size();
}

CSubRecord_Entry *CRecord_Entry::GetSubRecord(unsigned long toget)
{
	return listsubrecords[toget];
}

void CRecord_Entry::Add_SubRecord(CSubRecord_Entry *dasub)
{
	listsubrecords.push_back(dasub);
}

unsigned long CRecord_Entry::Get_RecordType()
{
	return record_type;
}

unsigned long CRecord_Entry::Get_FormID()
{
	return thisRecordHeader.formid;
}

char *CRecord_Entry::Get_EditorID()
{
	unsigned long index;
	CSubString_Entry *temp_edid;
	
	for(index=0;index<listsubrecords.size();index++)
	{
		if(listsubrecords[index]->Get_SubRecordType()==EDID)
		{
			temp_edid=(CSubString_Entry *)listsubrecords[index];
			return temp_edid->Get_String();
		}
	}
	return NULL;
}

unsigned long CRecord_Entry::Compare(RecordHeader daheader)
{
	if((daheader.formid==thisRecordHeader.formid)&&(daheader.type.long_type==thisRecordHeader.type.long_type))
	{
		return 1;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//LAND ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CLand_Record::CLand_Record(RecordHeader daheader,CModule *cur_mod) : CRecord_Entry(LAND,daheader,cur_mod)
{
}

CLand_Record::~CLand_Record()
{
}

unsigned long CLand_Record::IsThereVHGT()
{
	if(Get_VHGT()!=NULL) return 1;
	return 0;
}

CSubVHGT_Entry *CLand_Record::Get_VHGT()
{
	unsigned long index;
	for(index=0;index<listsubrecords.size();index++)
	{
		if(listsubrecords[index]->Get_SubRecordType()==VHGT)
		{
			return (CSubVHGT_Entry *)listsubrecords[index];
		}
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//WRLD ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CWrld_Record::CWrld_Record(RecordHeader daheader,CModule *cur_mod) : CRecord_Entry(WRLD,daheader,cur_mod)
{
	associated_grup=NULL;
}

CWrld_Record::~CWrld_Record()
{
}

void CWrld_Record::SetAssociatedGrup(CGrup_Entry *association)
{
	associated_grup=association;
}

CGrup_Entry *CWrld_Record::GetAssociatedGrup()
{
	return associated_grup;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CELL ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCell_Record::CCell_Record(RecordHeader daheader,CModule *cur_mod) : CRecord_Entry(CELL,daheader,cur_mod)
{
	associated_grup=NULL;
}

CCell_Record::~CCell_Record()
{
}

void CCell_Record::SetAssociatedGrup(CGrup_Entry *association)
{
	associated_grup=association;
}

CGrup_Entry *CCell_Record::GetAssociatedGrup()
{
	return associated_grup;
}

CLand_Record *CCell_Record::GetAssociatedLand()
{
	unsigned long index,index2;
	CGrup_Entry *tempcellchild_grup;
	CRecord_Entry *temp_record;

	if(!associated_grup) return NULL;

	for(index=0;index<associated_grup->GetNumGrups();index++)
	{
		tempcellchild_grup=associated_grup->GetGrup(index);
		if(tempcellchild_grup->Get_GroupType()==CELL_TEMP_CHILD)
		{
			for(index2=0;index2<tempcellchild_grup->GetNumRecords();index2++)
			{
				temp_record=tempcellchild_grup->GetRecord(index2);
				if(temp_record->Get_RecordType()==LAND) return (CLand_Record *)temp_record;
			}
		}
	}

	return NULL;
}

unsigned long CCell_Record::Get_XY(long &_X, long &_Y)
{
	unsigned long index;
	CSubXCLC_Entry *temp_xclc;

	_X=0;
	_Y=0;

	for(index=0;index<listsubrecords.size();index++)
	{
		if(listsubrecords[index]->Get_SubRecordType()==XCLC)
		{
			temp_xclc=(CSubXCLC_Entry *)listsubrecords[index];
			temp_xclc->Get_XY(_X,_Y);
			return 1;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//LTEX ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CLtex_Record::CLtex_Record(RecordHeader daheader, CModule *cur_mod) : CRecord_Entry(LTEX,daheader,cur_mod)
{
}

CLtex_Record::~CLtex_Record()
{
}

char *CLtex_Record::Get_Filename()
{
	unsigned long index;
	CSubString_Entry *daicon;

	for(index=0;index<listsubrecords.size();index++)
	{
		if(listsubrecords[index]->Get_SubRecordType()==ICON)
		{
			daicon=(CSubString_Entry *)listsubrecords[index];
			return daicon->Get_String();
		}
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STAT ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CStat_Record::CStat_Record(RecordHeader daheader, CModule *cur_mod) : CRecord_Entry(STAT,daheader,cur_mod)
{
}

CStat_Record::~CStat_Record()
{
}

char *CStat_Record::Get_Filename()
{
	unsigned long index;
	CSubString_Entry *damodl;

	for(index=0;index<listsubrecords.size();index++)
	{
		if(listsubrecords[index]->Get_SubRecordType()==MODL)
		{
			damodl=(CSubString_Entry *)listsubrecords[index];
			return damodl->Get_String();
		}
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//REFR ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRefr_Record::CRefr_Record(RecordHeader daheader, CModule *cur_mod) : CRecord_Entry(REFR,daheader,cur_mod)
{
}

CRefr_Record::~CRefr_Record()
{
}

float *CRefr_Record::Get_PosAndRot()
{
	unsigned long index;

	for(index=0;index<listsubrecords.size();index++)
	{
		if(listsubrecords[index]->Get_SubRecordType()==DATA)
		{
			return static_cast<CSubDATA_Entry *>(listsubrecords[index])->Get_PosAndRot();
		}
	}
	return NULL;
}

float CRefr_Record::Get_Scale()
{
	unsigned long index;

	for(index=0;index<listsubrecords.size();index++)
	{
		if(listsubrecords[index]->Get_SubRecordType()==XSCL)
		{
			return static_cast<CSubFloat_Entry *>(listsubrecords[index])->Get_Float();
		}
	}
	return 1.0f;
}

unsigned long CRefr_Record::Get_Reference()
{
	unsigned long index;
	CSubFormID_Entry *daform;

	for(index=0;index<listsubrecords.size();index++)
	{
		if(listsubrecords[index]->Get_SubRecordType()==NAME)
		{
			daform=(CSubFormID_Entry *)listsubrecords[index];
			return daform->Get_FormID();
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SUBRECORD ENTRY///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSubRecord_Entry::CSubRecord_Entry(unsigned long _type,SubRecordHeader daheader)
{
	memcpy(&thisSubRecordHeader,&daheader,sizeof(SubRecordHeader));
	subrecord_type=_type;
	data_pointer=NULL;
}

CSubRecord_Entry::CSubRecord_Entry(SubRecordHeader daheader,unsigned char *data)
{
	memcpy(&thisSubRecordHeader,&daheader,sizeof(SubRecordHeader));
	subrecord_type=UNKNOWN_SUBRECORD;

	data_pointer=new unsigned char[thisSubRecordHeader.size];
	memcpy(data_pointer,data,thisSubRecordHeader.size);
}

CSubRecord_Entry::~CSubRecord_Entry()
{
	if(data_pointer) delete [] data_pointer;
	data_pointer=NULL;
}

unsigned long CSubRecord_Entry::Get_SubRecordType()
{
	return subrecord_type;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CSUBFORMID ENTRY//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSubFormID_Entry::CSubFormID_Entry(unsigned long _type, SubRecordHeader daheader, unsigned char *data, CModule *cur_mod) : CSubRecord_Entry(_type,daheader)
{
	formID=cur_mod->TranslateFormID(((unsigned long *)data)[0]);
}

CSubFormID_Entry::~CSubFormID_Entry()
{
}

unsigned long CSubFormID_Entry::Get_FormID()
{
	return formID;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CSUBFLOAT ENTRY///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSubFloat_Entry::CSubFloat_Entry(unsigned long _type, SubRecordHeader daheader, unsigned char *data) : CSubRecord_Entry(_type,daheader)
{
	dafloat=((float *)data)[0];
}

CSubFloat_Entry::~CSubFloat_Entry()
{
}

float CSubFloat_Entry::Get_Float()
{
	return dafloat;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CSUBSTRING ENTRY//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSubString_Entry::CSubString_Entry(unsigned long _type, SubRecordHeader daheader, unsigned char *data) : CSubRecord_Entry(_type,daheader)
{
	unsigned long length;

	length=(unsigned long)strlen((char *)data)+1;
	thestring=new char[length];
	strcpy_s(thestring,length,(char *)data);
}

CSubString_Entry::~CSubString_Entry()
{
	if(thestring) delete [] thestring;
}

char *CSubString_Entry::Get_String()
{
	return thestring;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//XCLR ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSubXCLR_Entry::CSubXCLR_Entry(SubRecordHeader daheader, unsigned char *data,CModule *cur_mod) : CSubRecord_Entry(XCLR,daheader)
{
	unsigned long index;
	for(index=0;index<(unsigned long)(daheader.size/4);index++)
	{
		listformIDs.push_back(cur_mod->TranslateFormID(((unsigned long *)&data[index*4])[0]));
	}
}

CSubXCLR_Entry::~CSubXCLR_Entry()
{
	listformIDs.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ATXT ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSubATXT_Entry::CSubATXT_Entry(SubRecordHeader daheader, unsigned char *data, CModule *cur_mod) : CSubRecord_Entry(ATXT,daheader)
{
	formID=cur_mod->TranslateFormID(((unsigned long *)data)[0]);
	quadrant=data[4];
	layer=((unsigned short *)(&data[6]))[0];
}

CSubATXT_Entry::~CSubATXT_Entry()
{
}

unsigned long CSubATXT_Entry::Get_TextureFormID(unsigned char &_quadrant,unsigned short &_layer)
{
	_quadrant=quadrant;
	_layer=layer;
	return formID;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BTXT ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSubBTXT_Entry::CSubBTXT_Entry(SubRecordHeader daheader, unsigned char *data, CModule *cur_mod) : CSubRecord_Entry(BTXT,daheader)
{
	formID=cur_mod->TranslateFormID(((unsigned long *)data)[0]);
	quadrant=data[4];
}

CSubBTXT_Entry::~CSubBTXT_Entry()
{
}

unsigned long CSubBTXT_Entry::Get_TextureFormID(unsigned char &_quadrant)
{
	_quadrant=quadrant;
	return formID;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//XCLC ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSubXCLC_Entry::CSubXCLC_Entry(SubRecordHeader daheader,unsigned char *data) : CSubRecord_Entry(XCLC,daheader)
{
	memcpy(&X,&data[0],4);
	memcpy(&Y,&data[4],4);
}

CSubXCLC_Entry::~CSubXCLC_Entry()
{
}

void CSubXCLC_Entry::Get_XY(long &_X,long &_Y)
{
	_X=X;
	_Y=Y;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//VHGT ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSubVHGT_Entry::CSubVHGT_Entry(SubRecordHeader daheader,unsigned char *data) : CSubRecord_Entry(VHGT,daheader)
{
	height_data=new char[1089];

	memcpy(&offset,data,4);
	memcpy(height_data,&data[4],1089);
}

CSubVHGT_Entry::~CSubVHGT_Entry()
{
	if(height_data) delete [] height_data;
}

char *CSubVHGT_Entry::Get_Height(float &_offset)
{
	_offset=offset;
	return height_data;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//VNML ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSubVNML_Entry::CSubVNML_Entry(SubRecordHeader daheader, unsigned char *data) : CSubRecord_Entry(VNML,daheader)
{
	normal_data=new char[3267];
	memcpy(normal_data,data,3267);
}

CSubVNML_Entry::~CSubVNML_Entry()
{
	if(normal_data) delete [] normal_data;
}

char *CSubVNML_Entry::Get_Normals()
{
	return normal_data;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//VNML ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSubVCLR_Entry::CSubVCLR_Entry(SubRecordHeader daheader, unsigned char *data) : CSubRecord_Entry(VCLR,daheader)
{
	color_data=new unsigned char[3267];
	memcpy(color_data,data,3267);
}

CSubVCLR_Entry::~CSubVCLR_Entry()
{
	if(color_data) delete [] color_data;
}

unsigned char *CSubVCLR_Entry::Get_Colors()
{
	return color_data;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//VNML ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSubVTXT_Entry::CSubVTXT_Entry(SubRecordHeader daheader, unsigned char *data) : CSubRecord_Entry(VTXT,daheader)
{
	unsigned long index;

	num_entries=(daheader.size/8);
	positions=new unsigned short[num_entries];
	opacities_data=new float[num_entries];

	for(index=0;index<num_entries;index++)
	{
		memcpy(&positions[index],&data[index*8],2);
		memcpy(&opacities_data[index],&data[(index*8)+4],4);
	}
}

CSubVTXT_Entry::~CSubVTXT_Entry()
{
	if(positions) delete [] positions;
	if(opacities_data) delete [] opacities_data;
}

unsigned long CSubVTXT_Entry::Get_Opacities(unsigned short *&_positions,float *&_opacities)
{
	_positions=positions;
	_opacities=opacities_data;
	return num_entries;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//DATA ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSubDATA_Entry::CSubDATA_Entry(SubRecordHeader daheader, unsigned char *data) : CSubRecord_Entry(DATA,daheader)
{
	memcpy(posandrot,data,sizeof(float)*6);
}

CSubDATA_Entry::~CSubDATA_Entry()
{
}

float *CSubDATA_Entry::Get_PosAndRot()
{
	return posandrot;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//GRUP ENTRY////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Constructor only for tree root
CGrup_Entry::CGrup_Entry()
{
	listgrups.clear();
	listrecords.clear();
}

CGrup_Entry::CGrup_Entry(GrupHeader init,CModule *cur_mod)
{
	memcpy(&thisGrupHeader,&init,sizeof(GrupHeader));
	if(thisGrupHeader.groupType==WORLD_CHILD||thisGrupHeader.groupType==CELL_CHILD||thisGrupHeader.groupType==TOPIC_CHILD||thisGrupHeader.groupType==CELL_PERS_CHILD||
		thisGrupHeader.groupType==CELL_TEMP_CHILD||thisGrupHeader.groupType==CELL_VISI_DIST)
	{
		thisGrupHeader.label.formID=cur_mod->TranslateFormID(thisGrupHeader.label.formID);
	}
		
	CGrup_Entry();
}

CGrup_Entry::~CGrup_Entry()
{
	unsigned long index;
	
	//Delete all grup children
	for(index=0;index<listgrups.size();index++)
	{
		delete listgrups[index];
	}
	listgrups.clear();
	//Delete all record children
	for(index=0;index<listrecords.size();index++)
	{
		delete listrecords[index];
	}
	listrecords.clear();
}

CGrup_Entry *CGrup_Entry::doesGrupExists(GrupHeader daheader,CModule *cur_mod)
{
	unsigned long index;
	GrupHeader tempHeader;

	memcpy(&tempHeader,&daheader,sizeof(GrupHeader));
	if(tempHeader.groupType==WORLD_CHILD||tempHeader.groupType==CELL_CHILD||tempHeader.groupType==TOPIC_CHILD||tempHeader.groupType==CELL_PERS_CHILD||
		tempHeader.groupType==CELL_TEMP_CHILD||tempHeader.groupType==CELL_VISI_DIST)
	{
		tempHeader.label.formID=cur_mod->TranslateFormID(tempHeader.label.formID);
	}

	for(index=0;index<listgrups.size();index++)
	{
		if(listgrups[index]->Compare(tempHeader)) return listgrups[index];
	}
	return NULL;
}

CRecord_Entry *CGrup_Entry::doesRecordExists(RecordHeader daheader,CModule *cur_mod)
{
	unsigned long index;
	RecordHeader tempHeader;

	memcpy(&tempHeader,&daheader,sizeof(RecordHeader));
	tempHeader.formid=cur_mod->TranslateFormID(daheader.formid);

	for(index=0;index<listrecords.size();index++)
	{
		if(listrecords[index]->Compare(tempHeader)) return listrecords[index];
	}
	return NULL;
}

unsigned long CGrup_Entry::Compare(GrupHeader daheader)
{
	//Not memcmp because of timestamp in the header
	if((thisGrupHeader.groupType==daheader.groupType)&&(thisGrupHeader.label.formID==daheader.label.formID))
	{
		return 1;
	}
	return 0;
}

void CGrup_Entry::Add_Grup(CGrup_Entry *toadd)
{
	listgrups.push_back(toadd);
}

void CGrup_Entry::Add_Record(CRecord_Entry *toadd)
{
	listrecords.push_back(toadd);
}

void CGrup_Entry::Swap_Record(CRecord_Entry *oldrec,CRecord_Entry *newrec)
{
	unsigned long index;
	
	for(index=0;index<listrecords.size();index++)
	{
		if(listrecords[index]==oldrec)
		{
			delete listrecords[index];
			listrecords[index]=newrec;
			return;
		}
	}

	throw L"Couldn't find the record to swap...";
}

void CGrup_Entry::Delete_Record(CRecord_Entry *todel)
{
	unsigned long index;

	for(index=0;index<listrecords.size();index++)
	{
		if(listrecords[index]==todel)
		{
			delete listrecords[index];
			listrecords.erase(listrecords.begin()+index);
			return;
		}
	}

	throw L"Couldn't find the record to delete...";
}

void CGrup_Entry::Delete_Grup(CGrup_Entry *todel)
{
	unsigned long index;
	for(index=0;index<listgrups.size();index++)
	{
		if(listgrups[index]==todel)
		{
			delete listgrups[index];
			listgrups.erase(listgrups.begin()+index);
			return;
		}
	}

	throw L"Couldn't find the record to delete...";
}

unsigned long CGrup_Entry::Get_GroupType()
{
	return thisGrupHeader.groupType;
}

unsigned long CGrup_Entry::GetNumGrups()
{
	return (unsigned long)listgrups.size();
}

CGrup_Entry *CGrup_Entry::GetGrup(unsigned long toget)
{
	return listgrups[toget];
}

unsigned long CGrup_Entry::GetNumRecords()
{
	return (unsigned long)listrecords.size();
}

CRecord_Entry *CGrup_Entry::GetRecord(unsigned long toget)
{
	return listrecords[toget];
}

CGrup_Entry *CGrup_Entry::Find_Grup(GrupHeader daheader)
{
	unsigned long index;

	for(index=0;index<listgrups.size();index++)
	{
		if(listgrups[index]->Compare(daheader)) return listgrups[index];
	}
	return NULL;
}

unsigned long CGrup_Entry::Get_FormID()
{
	return thisGrupHeader.label.formID;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TREE MANAGER//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CTree_Root::CTree_Root()
{
}

CTree_Root::~CTree_Root()
{
}

CGrup_Entry *CTree_Root::Get_TopGrup(char *grup_name)
{
	GrupHeader dagrup;

	dagrup.groupType=TOP;
	memcpy(dagrup.label.label_name,grup_name,4);
	return Find_Grup(dagrup);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TREE MANAGER//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CTree_Manager::CTree_Manager()
{
	tree_root=new CTree_Root();
}

CTree_Manager::~CTree_Manager()
{
	if(tree_root) delete tree_root;
}

//Maybe remove the compressed parameter and use function for record entry instead?
void CTree_Manager::LoadTreeRawRecord(unsigned char *buffer, unsigned long size, CRecord_Entry *cur_record,unsigned long compressed,CModule *cur_mod)
{
	unsigned long curpointer,index;
	SubRecordHeader srec_header;

	unsigned char *decbuf;
	unsigned long decsize;
	int dec_result;


	CSubRecord_Entry *temp_subrecord;

	if(compressed!=0)
	{
		decsize=((unsigned long *)buffer)[0];
		decbuf=new unsigned char[decsize];
		dec_result=uncompress(decbuf,&decsize,&buffer[4],size);
		if(dec_result!=Z_OK)
		{
			output_text(L"Error uncompressing!\r\n");
			switch(dec_result)
			{
			case Z_MEM_ERROR:
				output_text(L"ZLib:Not enough memory!\r\n");
				break;
			case Z_BUF_ERROR:
				output_text(L"Zlib:Not enough space in output buffer!\r\n");
				break;
			case Z_DATA_ERROR:
				output_text(L"Zlib:Data corrupted!\r\n");
				break;
			default:
				output_text(L"Zlib:Unknown error!\r\n");
				break;
			}
			return;
		}
		else
		{
			buffer=decbuf;
			size=decsize;
		}
	}
	
	curpointer=0;
	while(curpointer<size)
	{
		//Records only contain subrecords
		memcpy(&srec_header,&buffer[curpointer],6);

		for(index=0;index<NUM_SUBTAGS;index++)
		{
			if(strncmp((char *)&buffer[curpointer],SubTag_Records[index],4)==0)
			{
				switch((index+1000))
				{
				//Special subrecords first
				case XCLC:
					temp_subrecord=new CSubXCLC_Entry(srec_header,&buffer[curpointer+6]);
					break;
				case VHGT:
					temp_subrecord=new CSubVHGT_Entry(srec_header,&buffer[curpointer+6]);
					break;
				case XCLR:
					temp_subrecord=new CSubXCLR_Entry(srec_header,&buffer[curpointer+6],cur_mod);
					break;
				case ATXT:
					temp_subrecord=new CSubATXT_Entry(srec_header,&buffer[curpointer+6],cur_mod);
					break;
				case BTXT:
					temp_subrecord=new CSubBTXT_Entry(srec_header,&buffer[curpointer+6],cur_mod);
					break;
				case VNML:
					temp_subrecord=new CSubVNML_Entry(srec_header,&buffer[curpointer+6]);
					break;
				case VTXT:
					temp_subrecord=new CSubVTXT_Entry(srec_header,&buffer[curpointer+6]);
					break;
				case VCLR:
					temp_subrecord=new CSubVCLR_Entry(srec_header,&buffer[curpointer+6]);
					break;
				case DATA:
					temp_subrecord=new CSubDATA_Entry(srec_header,&buffer[curpointer+6]);
					break;
				//Float subrecords
				case XSCL:
					temp_subrecord=new CSubFloat_Entry(XSCL,srec_header,&buffer[curpointer+6]);
					break;
				//String subrecords
				case EDID:
					temp_subrecord=new CSubString_Entry(EDID,srec_header,&buffer[curpointer+6]);
					break;
				case ICON:
					temp_subrecord=new CSubString_Entry(ICON,srec_header,&buffer[curpointer+6]);
					break;
				case MODL:
					temp_subrecord=new CSubString_Entry(MODL,srec_header,&buffer[curpointer+6]);
					break;
				//FormID subrecords
				case XOWN:
					temp_subrecord=new CSubFormID_Entry(XOWN,srec_header,&buffer[curpointer+6],cur_mod);
					break;
				case XGLB:
					temp_subrecord=new CSubFormID_Entry(XGLB,srec_header,&buffer[curpointer+6],cur_mod);
					break;
				case XCCM:
					temp_subrecord=new CSubFormID_Entry(XCCM,srec_header,&buffer[curpointer+6],cur_mod);
					break;
				case XCWT:
					temp_subrecord=new CSubFormID_Entry(XCWT,srec_header,&buffer[curpointer+6],cur_mod);
					break;
				case WNAM:
					temp_subrecord=new CSubFormID_Entry(WNAM,srec_header,&buffer[curpointer+6],cur_mod);
					break;
				case CNAM:
					temp_subrecord=new CSubFormID_Entry(CNAM,srec_header,&buffer[curpointer+6],cur_mod);
					break;
				case NAM2:
					temp_subrecord=new CSubFormID_Entry(NAM2,srec_header,&buffer[curpointer+6],cur_mod);
					break;
				case NAME:
					temp_subrecord=new CSubFormID_Entry(NAME,srec_header,&buffer[curpointer+6],cur_mod);
					break;
				default:
					//SubTag is in the list but not handled yet
					output_text(L"SubRecord is in list but not handled, this should not happen...\r\n");
					throw L"Unknown subrecord!";
					temp_subrecord=new CSubRecord_Entry(srec_header,&buffer[curpointer+6]);
					break;
				}
				//Add to the tree
				cur_record->Add_SubRecord(temp_subrecord);
				break;
			}
		}
		if(index==NUM_SUBTAGS)
		{
			//Tag not in the list and so not handled, store it anyway
			temp_subrecord=new CSubRecord_Entry(srec_header,&buffer[curpointer+6]);
			cur_record->Add_SubRecord(temp_subrecord);
		}
		//Go to the next subrecord
		curpointer+=(srec_header.size+6);
	}

	if(compressed!=0&&dec_result==Z_OK)
	{
		delete [] decbuf;
	}
}

//TOADD:Look if it is an already existing entry
//TOADD:Split the grup between different threads to speed up
void CTree_Manager::LoadTreeRawGrup(unsigned char *buffer,unsigned long size, CGrup_Entry *cur_grup,CModule *cur_mod)
{
	unsigned long curpointer;
	unsigned long index;

	GrupHeader grup_header;
	RecordHeader rec_header;

	CGrup_Entry *existing_grup;
	CGrup_Entry *temp_grup;
	CRecord_Entry *existing_record;
	CRecord_Entry *temp_record;

	if(cur_grup==NULL) cur_grup=tree_root;

	curpointer=0;
	while(curpointer<size)
	{
		//Is tag GRUP?
		if(strncmp((char *)&buffer[curpointer],"GRUP",4)==0)
		{
			//Read Grup Header
			memcpy(&grup_header,&buffer[curpointer],20);
			//Check if it already exists
			existing_grup=cur_grup->doesGrupExists(grup_header,cur_mod);
			if(existing_grup==NULL)
			{
				temp_grup=new CGrup_Entry(grup_header,cur_mod);
				cur_grup->Add_Grup(temp_grup);
				LoadTreeRawGrup(&buffer[curpointer+20],(grup_header.groupsize-20),temp_grup,cur_mod);
			}
			else
			{
				LoadTreeRawGrup(&buffer[curpointer+20],(grup_header.groupsize-20),existing_grup,cur_mod);
			}
			//Go to the next tag
			curpointer+=grup_header.groupsize;
		}
		else
		{
			//Not Grup so it's a record
			memcpy(&rec_header,&buffer[curpointer],20);
			for(index=0;index<NUM_TAGS;index++)
			{
				//Could be improved...
				if(strncmp((char *)&buffer[curpointer],Tag_Records[index],4)==0)
				{
					switch((index+1))
					{
					case WRLD:
						temp_record=new CWrld_Record(rec_header,cur_mod);
						break;
					case LAND:
						temp_record=new CLand_Record(rec_header,cur_mod);
						break;
					case CELL:
						temp_record=new CCell_Record(rec_header,cur_mod);
						break;
					case LTEX:
						temp_record=new CLtex_Record(rec_header,cur_mod);
						break;
					case STAT:
						temp_record=new CStat_Record(rec_header,cur_mod);
						break;
					case REFR:
						temp_record=new CRefr_Record(rec_header,cur_mod);
						break;
					default:
						//Tag is in the list but not handled yet
						temp_record=NULL;
						break;
					}
					break;
				}
			}
			if(index==NUM_TAGS)
			{
				temp_record=NULL;
				output_text(L"Unknown Record Tag, this should not happen!\r\n");
			}

			if(temp_record!=NULL)
			{
				existing_record=cur_grup->doesRecordExists(rec_header,cur_mod);

				if(existing_record==NULL) cur_grup->Add_Record(temp_record);
				else cur_grup->Swap_Record(existing_record,temp_record);

				LoadTreeRawRecord(&buffer[curpointer+20],rec_header.size,temp_record,(rec_header.flags&COMPRESSED_FLAG),cur_mod);
			}
			curpointer+=(rec_header.size+20);
		}
	}
}

CCell_Record *CTree_Manager::Get_Cell(CGrup_Entry *worldspace,long x,long y)
{
	unsigned long index;
	CGrup_Entry *block_grup,*subblock_grup;
	CRecord_Entry *temp_record;
	CCell_Record *cell_toret;
	GrupHeader dagrup;
	float temp_x,temp_y;
	short block_x,block_y,subblock_x,subblock_y;
	long toret_x,toret_y;

	temp_x=(float)x;
	temp_y=(float)y;
	temp_x/=8.0f;
	temp_y/=8.0f;
	subblock_x=(short)floor(temp_x);
	subblock_y=(short)floor(temp_y);

	temp_x=subblock_x;
	temp_y=subblock_y;
	temp_x/=4.0f;
	temp_y/=4.0f;
	block_x=(short)floor(temp_x);
	block_y=(short)floor(temp_y);

	//Find the Block
	dagrup.groupType=EXTER_CELL_BLOCK;
	dagrup.label.XY[0]=block_y;
	dagrup.label.XY[1]=block_x;
	block_grup=worldspace->Find_Grup(dagrup);
	if(!block_grup) return NULL;

	dagrup.groupType=EXTER_CELL_SUBBLOCK;
	dagrup.label.XY[0]=subblock_y;
	dagrup.label.XY[1]=subblock_x;
	subblock_grup=block_grup->Find_Grup(dagrup);
	if(!subblock_grup) return NULL;

	for(index=0;index<subblock_grup->GetNumRecords();index++)
	{
		temp_record=subblock_grup->GetRecord(index);
		if(temp_record->Get_RecordType()==CELL)
		{
			cell_toret=(CCell_Record *)temp_record;
			if(cell_toret->Get_XY(toret_x,toret_y))
			{
				if(toret_x==x&&toret_y==y) return cell_toret;
			}
		}
	}

	return NULL;
}

void CTree_Manager::Optimize_Worldspace(CGrup_Entry *worldspace)
{
	unsigned long index,index2,index3;
	CGrup_Entry *block_grup,*subblock_grup,*cell_grup;
	
	CRecord_Entry *temp_record;
	CCell_Record *cell_record;
	GrupHeader dagrup;

	for(index=0;index<worldspace->GetNumGrups();index++)
	{
		block_grup=worldspace->GetGrup(index);
		//There is also a CELL and a the grup attached to it here for global world stuff
		if(block_grup->Get_GroupType()==EXTER_CELL_BLOCK)
		{
			for(index2=0;index2<block_grup->GetNumGrups();index2++)
			{
				subblock_grup=block_grup->GetGrup(index2);
				if(subblock_grup->Get_GroupType()!=EXTER_CELL_SUBBLOCK) throw L"Strange non-subblock grup in the block grup!";
	
				for(index3=0;index3<subblock_grup->GetNumRecords();index3++)
				{
					temp_record=subblock_grup->GetRecord(index3);
					if(temp_record->Get_RecordType()!=CELL) throw L"Record in subblock isn't a CELL!";
	
					cell_record=(CCell_Record *)temp_record;
	
					dagrup.groupType=CELL_CHILD;
					dagrup.label.formID=cell_record->Get_FormID();
	
					cell_grup=subblock_grup->Find_Grup(dagrup);
					//Sometime there is no grup associated to the cell...
					if(cell_grup) cell_record->SetAssociatedGrup(cell_grup);
				}
			}
		}
	}

	//Linking the global cell to its grup
	for(index=0;index<worldspace->GetNumRecords();index++)
	{
		temp_record=worldspace->GetRecord(index);
		if(temp_record->Get_RecordType()!=CELL) throw L"Presence of a record which isn't a CELL in the worldspace!";
		cell_record=(CCell_Record *)temp_record;

		dagrup.groupType=CELL_CHILD;
		dagrup.label.formID=cell_record->Get_FormID();

		cell_grup=worldspace->Find_Grup(dagrup);
		if(cell_grup) cell_record->SetAssociatedGrup(cell_grup);
	}
}

void CTree_Manager::Add_To_Lookup(CGrup_Entry *cur_grup)
{
	unsigned long index,rformID;
	lookup_entry *dalookup;
	CRecord_Entry *temp_record;

	for(index=0;index<cur_grup->GetNumRecords();index++)
	{
		temp_record=cur_grup->GetRecord(index);
		rformID=temp_record->Get_FormID()&0xFFFFFF;

		dalookup=lookup_formID[rformID];
		if(dalookup==NULL)
		{
			lookup_formID[rformID]=new lookup_entry;
			dalookup=lookup_formID[rformID];
		}
		else
		{
			while(dalookup->next_stuff!=NULL) dalookup=dalookup->next_stuff;
			dalookup->next_stuff=new lookup_entry;
			dalookup=dalookup->next_stuff;
		}

		dalookup->next_stuff=NULL;
		dalookup->cur_entry=temp_record;
	}
}


void CTree_Manager::Optimize_Tree()
{
	unsigned long index;
	CGrup_Entry *worldspace_grup;
	CRecord_Entry *temp_record;
	CWrld_Record *world_record;

	GrupHeader dagrup;

	//Get some top grup
	pListWrld=tree_root->Get_TopGrup("WRLD");
	if(!pListWrld) throw L"Couldn't find WRLD top grup!";
	pListLtex=tree_root->Get_TopGrup("LTEX");
	if(!pListLtex) throw L"Couldn't find LTEX top grup!";
	pListStat=tree_root->Get_TopGrup("STAT");
	if(!pListStat) throw L"Couldn't find STAT top grup!";

	//Link worldspaces to their grup
	dagrup.groupType=WORLD_CHILD;
	for(index=0;index<pListWrld->GetNumRecords();index++)
	{
		temp_record=pListWrld->GetRecord(index);
		if(temp_record->Get_RecordType()==WRLD)
		{
			world_record=(CWrld_Record *)temp_record;
			dagrup.label.formID=world_record->Get_FormID();
			worldspace_grup=pListWrld->Find_Grup(dagrup);
			//Sometimes no grup is associated to the worldspace;
			if(worldspace_grup)
			{
				world_record->SetAssociatedGrup(worldspace_grup);
				//While we're at it let's optimize the worldspace
				Optimize_Worldspace(worldspace_grup);
			}
		}
		else output_text(L"Strange, a non WRLD record in the WRLD top grup...\r\n");
	}

	//Build lookup table
	lookup_formID=new lookup_entry *[0x1000000];
	memset(lookup_formID,0,sizeof(lookup_entry *)*0x1000000);
	Add_To_Lookup(pListStat);
	Add_To_Lookup(pListLtex);
}

CLtex_Record *CTree_Manager::GetLandscapeTexture(unsigned long want_formID)
{
	lookup_entry *cur_lookup;

	cur_lookup=lookup_formID[want_formID&0xFFFFFF];
	while(cur_lookup)
	{
		if((cur_lookup->cur_entry->Get_FormID()==want_formID)&&(cur_lookup->cur_entry->Get_RecordType()==LTEX)) return (CLtex_Record *)cur_lookup->cur_entry;
		cur_lookup=cur_lookup->next_stuff;
	}
	return NULL;
}

CGrup_Entry *CTree_Manager::GetWorldGrup(unsigned long formid)
{
	unsigned long index;
	CRecord_Entry *darec;
	CWrld_Record *dawrld;

	for(index=0;index<pListWrld->GetNumRecords();index++)
	{
		darec=pListWrld->GetRecord(index);
		if((darec->Get_RecordType()==WRLD)&&(darec->Get_FormID()==formid))
		{
			dawrld=(CWrld_Record *)darec;
			return dawrld->GetAssociatedGrup();
		}
	}
	return NULL;
}

void CTree_Manager::SetWorldspace(unsigned long fid_WorldSpace)
{
	pCurWrld=pTreeManager->GetWorldGrup(fid_WorldSpace);
}

CCell_Record *CTree_Manager::GetCurWrldCell(long x,long y)
{
	return Get_Cell(pCurWrld,x,y);
}

CStat_Record *CTree_Manager::GetStat(unsigned long want_formID)
{
	lookup_entry *cur_lookup;

	cur_lookup=lookup_formID[want_formID&0xFFFFFF];
	while(cur_lookup)
	{
		if((cur_lookup->cur_entry->Get_FormID()==want_formID)&&(cur_lookup->cur_entry->Get_RecordType()==STAT)) return (CStat_Record *)cur_lookup->cur_entry;
		cur_lookup=cur_lookup->next_stuff;
	}
	return NULL;
}
