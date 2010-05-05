#ifndef TREEMANAGER_HEADER
#define TREEMANAGER_HEADER

#define COMPRESSED_FLAG 0x00040000

//Grup
#define GRUP 0
//Records
#define ACHR 1 
#define ACRE 2 
#define ACTI 3 
#define ALCH 4 
#define AMMO 5 
#define ANIO 6 
#define APPA 7 
#define ARMO 8 
#define BOOK 9 
#define BSGN 10
#define CELL 11
#define CLAS 12
#define CLMT 13
#define CLOT 14
#define CONT 15
#define CREA 16
#define CSTY 17
#define DIAL 18
#define DOOR 19
#define EFSH 20
#define ENCH 21
#define EYES 22
#define FACT 23
#define FLOR 24
#define FURN 25
#define GLOB 26
#define GMST 27
#define GRAS 28
#define HAIR 29
#define IDLE 30
#define INFO 31
#define INGR 32
#define KEYM 33
#define LAND 34
#define LIGH 35
#define LSCR 36
#define LTEX 37
#define LVLC 38
#define LVLI 39
#define LVSP 40
#define MGEF 41
#define MISC 42
#define NPC_ 43
#define PACK 44
#define PGRD 45
#define QUST 46
#define RACE 47
#define REFR 48
#define REGN 49
#define ROAD 50
#define SBSP 51
#define SCPT 52
#define SGST 53
#define SKIL 54
#define SLGM 55
#define SOUN 56
#define SPEL 57
#define STAT 58
#define TES4 59
#define TREE 60
#define WATR 61
#define WEAP 62
#define WRLD 63
#define WTHR 64
//List of subrecords
#define XCLC 1000
#define VHGT 1001
#define EDID 1002
#define XOWN 1003
#define XGLB 1004
#define XCCM 1005
#define XCWT 1006
#define XCLR 1007
#define WNAM 1008
#define CNAM 1009
#define NAM2 1010
#define ATXT 1011
#define BTXT 1012
#define ICON 1013
#define VNML 1014
#define VCLR 1015
#define VTXT 1016
#define MODL 1017
#define NAME 1018
#define DATA 1019
#define XSCL 1020
//Error filter
#define UNKNOWN 255
#define UNKNOWN_SUBRECORD 256

//List of GroupTypes
#define TOP 0
#define WORLD_CHILD 1
#define INTER_CELL_BLOCK 2
#define INTER_CELL_SUBBLOCK 3
#define EXTER_CELL_BLOCK 4
#define EXTER_CELL_SUBBLOCK 5
#define CELL_CHILD 6
#define TOPIC_CHILD 7
#define CELL_PERS_CHILD 8
#define CELL_TEMP_CHILD 9
#define CELL_VISI_DIST 10

#pragma pack(1)

typedef struct {
	char type[4];
	unsigned long groupsize;
	union {
		char label_name[4];
		short XY[2];
		unsigned long formID;
	} label;
	unsigned long groupType;
	unsigned long stamp;
} GrupHeader;

typedef struct {
	union {
		char char_type[4];
		unsigned long long_type;
	} type;
	unsigned long size;
	unsigned long flags;
	unsigned long formid;
	unsigned long flags_2;
} RecordHeader;

typedef struct {
	char type[4];
	unsigned short size;
} SubRecordHeader;

class CGrup_Entry;

#pragma pack()

class CModule;

//SUBRECORD ENTRY
class CSubRecord_Entry
{
public:
	//Constructor for tags handled
	CSubRecord_Entry(unsigned long,SubRecordHeader);
	//Constructor for tags not handled
	CSubRecord_Entry(SubRecordHeader,unsigned char *);
	//Destructor
	~CSubRecord_Entry();
	//Get the type of subrecord
	unsigned long Get_SubRecordType();
protected:
	SubRecordHeader thisSubRecordHeader;
	unsigned long subrecord_type;
	//Storage for unknown subrecords
	unsigned char *data_pointer;
};

//SUBRECORD ENTRY SPECIFIC FOR STUFF WHICH ONLY CONTAIN FORMID
class CSubFormID_Entry : public CSubRecord_Entry
{
public:
	CSubFormID_Entry(unsigned long,SubRecordHeader,unsigned char *,CModule *);
	~CSubFormID_Entry();
	unsigned long Get_FormID();
protected:
	unsigned long formID;
};

class CSubFloat_Entry : public CSubRecord_Entry
{
public:
	CSubFloat_Entry(unsigned long,SubRecordHeader,unsigned char *);
	~CSubFloat_Entry();
	float Get_Float();
protected:
	float dafloat;
};

class CSubString_Entry : public CSubRecord_Entry
{
public:
	CSubString_Entry(unsigned long,SubRecordHeader,unsigned char *);
	~CSubString_Entry();
	char *Get_String();
protected:
	char *thestring;
};

//XCLR ENTRY
class CSubXCLR_Entry : public CSubRecord_Entry
{
public:
	CSubXCLR_Entry(SubRecordHeader,unsigned char *,CModule *);
	~CSubXCLR_Entry();
protected:
	vector<unsigned long> listformIDs;
};

//ATXT ENTRY
class CSubATXT_Entry : public CSubRecord_Entry
{
public:
	CSubATXT_Entry(SubRecordHeader,unsigned char *,CModule *);
	~CSubATXT_Entry();
	//Get texture formID, quadrant and layer
	unsigned long Get_TextureFormID(unsigned char &,unsigned short &);
protected:
	unsigned long formID;
	unsigned char quadrant;
	unsigned short layer;
};

//BTXT ENTRY
class CSubBTXT_Entry : public CSubRecord_Entry
{
public:
	CSubBTXT_Entry(SubRecordHeader,unsigned char *,CModule *);
	~CSubBTXT_Entry();
	//Get texture formID and quadrant
	unsigned long Get_TextureFormID(unsigned char &);
protected:
	unsigned long formID;
	unsigned char quadrant;
};

//XCLC ENTRY
class CSubXCLC_Entry : public CSubRecord_Entry
{
public:
	CSubXCLC_Entry(SubRecordHeader,unsigned char *);
	~CSubXCLC_Entry();
	//Get X,Y from subrecord
	void Get_XY(long &,long &);
protected:
	long X,Y;
};

//VHGT ENTRY
class CSubVHGT_Entry : public CSubRecord_Entry
{
public:
	CSubVHGT_Entry(SubRecordHeader,unsigned char *);
	~CSubVHGT_Entry();
public:
	char *Get_Height(float &_offset);
protected:
	float offset;
	char *height_data;
	unsigned long modnumber;
};

//VCLR ENTRY
class CSubVCLR_Entry : public CSubRecord_Entry
{
public:
	CSubVCLR_Entry(SubRecordHeader,unsigned char *);
	~CSubVCLR_Entry();
public:
	unsigned char *Get_Colors();
protected:
	unsigned char *color_data;
};

//VNML ENTRY
class CSubVNML_Entry : public CSubRecord_Entry
{
public:
	CSubVNML_Entry(SubRecordHeader,unsigned char *);
	~CSubVNML_Entry();
public:
	char *Get_Normals();
protected:
	char *normal_data;
};

//VTXT ENTRY
class CSubVTXT_Entry : public CSubRecord_Entry
{
public:
	CSubVTXT_Entry(SubRecordHeader,unsigned char *);
	~CSubVTXT_Entry();
public:
	unsigned long Get_Opacities(unsigned short *&,float *&);
protected:
	unsigned short *positions;
	float *opacities_data;
	unsigned long num_entries;
};

class CSubDATA_Entry : public CSubRecord_Entry
{
public:
	CSubDATA_Entry(SubRecordHeader,unsigned char *);
	~CSubDATA_Entry();
public:
	float *Get_PosAndRot();
protected:
	float posandrot[6];
};

//GENERIC RECORD ENTRY
class CRecord_Entry
{
public:
	CRecord_Entry(unsigned long,RecordHeader,CModule *);
	~CRecord_Entry();
	//Add a SubRecord
	void Add_SubRecord(CSubRecord_Entry *);
	//Get the record type
	unsigned long Get_RecordType();
	//Get the FormID
	inline unsigned long Get_FormID();
	//Get the EditorID
	char *Get_EditorID();
	//Is it this record?
	unsigned long Compare(RecordHeader);
	//Get the number of subrecords
	unsigned long GetNumSubRecords();
	//Get a subrecord
	CSubRecord_Entry *GetSubRecord(unsigned long);
protected:
	RecordHeader thisRecordHeader;
	//List of records
	vector<CSubRecord_Entry *> listsubrecords;
	unsigned long record_type;
};


//LAND ENTRY
class CLand_Record : public CRecord_Entry
{
public:
	CLand_Record(RecordHeader,CModule *);
	~CLand_Record();
	unsigned long IsThereVHGT();
protected:
	CSubVHGT_Entry *Get_VHGT();
protected:
};

//WORLD ENTRY
class CWrld_Record : public CRecord_Entry
{
public:
	CWrld_Record(RecordHeader,CModule *);
	~CWrld_Record();
	void SetAssociatedGrup(CGrup_Entry *);
	CGrup_Entry *GetAssociatedGrup();
protected:
	CGrup_Entry *associated_grup;
};

//CELL ENTRY
class CCell_Record : public CRecord_Entry
{
public:
	CCell_Record(RecordHeader,CModule *);
	~CCell_Record();
	//Set the grup associated to the cell
	void SetAssociatedGrup(CGrup_Entry *);
	CGrup_Entry *GetAssociatedGrup();
	//Set the cell to modified
	void Set_IsModified();
	//Is the cell modified?
	unsigned long IsModified();
	//Get the coordinates of the cell if any
	unsigned long Get_XY(long &,long &);
	//Get the associated land record
	CLand_Record *GetAssociatedLand();
protected:
	CGrup_Entry *associated_grup;
};

class CLtex_Record : public CRecord_Entry
{
public:
	//constructor and destructor
	CLtex_Record(RecordHeader,CModule *);
	~CLtex_Record();
	//get the filename if any
	char *Get_Filename();
};

class CStat_Record : public CRecord_Entry
{
public:
	//constructor and destructor
	CStat_Record(RecordHeader,CModule *);
	~CStat_Record();
	//get the filename if any
	char *Get_Filename();
};

class CRefr_Record : public CRecord_Entry
{
public:
	CRefr_Record(RecordHeader,CModule *);
	~CRefr_Record();
	//Get the position and rotation if any
	float *Get_PosAndRot();
	//Get the scale
	float Get_Scale();
	//Get the formid this refr is referencing
	unsigned long Get_Reference();
};

//GRUP ENTRY
class CGrup_Entry
{
public:
	//Constructor and destructor
	CGrup_Entry();
	CGrup_Entry(GrupHeader,CModule *);
	~CGrup_Entry();
	//Search "sub" records and grups functions
	CGrup_Entry *doesGrupExists(GrupHeader,CModule *);
	CRecord_Entry *doesRecordExists(RecordHeader,CModule *);
	//Comparison function
	unsigned long Compare(GrupHeader);
	//Function to add grup or record
	void Add_Grup(CGrup_Entry *);
	void Add_Record(CRecord_Entry *);
	//Swap 2 records
	void Swap_Record(CRecord_Entry *,CRecord_Entry *);
	//Delete a record or a grup
	void Delete_Record(CRecord_Entry *);
	void Delete_Grup(CGrup_Entry *);
	//Iteration of Grups
	unsigned long GetNumGrups();
	CGrup_Entry *GetGrup(unsigned long);
	//Iteration of Records
	unsigned long GetNumRecords();
	CRecord_Entry *GetRecord(unsigned long);
	//Find a grup
	CGrup_Entry *Find_Grup(GrupHeader);
	//Extra information grabbers
	unsigned long Get_GroupType();
	unsigned long Get_FormID();
protected:
	GrupHeader thisGrupHeader;

	vector<CRecord_Entry *> listrecords;
	vector<CGrup_Entry *> listgrups;
};

class CTree_Root : public CGrup_Entry
{
public:
	CTree_Root();
	~CTree_Root();
	CGrup_Entry *Get_TopGrup(char *);
protected:
};

typedef struct _llist
{
	CRecord_Entry *cur_entry;
	struct _llist *next_stuff;
} lookup_entry;

class CTree_Manager
{
public:
	CTree_Manager();
	~CTree_Manager();
public:
	//Load tree data from file
	void LoadTreeRawGrup(unsigned char *,unsigned long, CGrup_Entry *,CModule *);
	void LoadTreeRawRecord(unsigned char *,unsigned long, CRecord_Entry *,unsigned long,CModule *);
	//Optimize the tree(linking grups with their associated record)
	void Optimize_Tree();
	//Unpack/Repack the HeightMaps for every exterior cell
	void UnpackHeightMaps();

	//Everything here is for explicit use by the engine

	//Return a landscape texture record
	CLtex_Record *GetLandscapeTexture(unsigned long);
	//Set the worldspace
	void SetWorldspace(unsigned long);
	//Get a cell from the current worldspace
	CCell_Record *GetCurWrldCell(long,long);
	//Get a stat object
	CStat_Record *GetStat(unsigned long);
protected:
	void Optimize_Worldspace(CGrup_Entry *);
	void UnpackHeightMaps_Worldspace(CGrup_Entry *);
	CCell_Record *Get_Cell(CGrup_Entry *,long,long);
	//Return a pointer to the world grup(to deprecate?)
	CGrup_Entry *GetWorldGrup(unsigned long);
	//Add all the entries inside a grup to the lookup table
	void Add_To_Lookup(CGrup_Entry *);
protected:
	CTree_Root *tree_root;
	//optimisation of the access to the tree stuff
	CGrup_Entry *pListWrld,*pListLtex,*pCurWrld,*pListStat;
	//formid lookup table
	lookup_entry **lookup_formID;
};

#endif