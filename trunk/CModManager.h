#ifndef MODMANAGER_HEADER
#define MODMANAGER_HEADER

#include "CTreeManager.h"

typedef struct {
	char path[MAX_PATH];
	unsigned long type;
	FILETIME timestamp;
	unsigned long loaded;
} ModInfo;

class CModManager;

//Handle a single module
class CModule
{
public:
	CModule();
	~CModule();
	//Load module
	void LoadMod(HANDLE,CModManager *,char *);
	//Translate a formid from local to global scope
	unsigned long TranslateFormID(unsigned long);
protected:
	unsigned long LoadHeader(unsigned char *,unsigned long);
	void BuildFormidTranslation(CModManager *);
protected:
	RecordHeader *main_header;
	vector<char *> masterlist;
	vector<unsigned long> local_to_global_formid;
};


//Will lookup file timestamp and load in order
class CModManager
{
public:
	CModManager();
	~CModManager();
	//Init the mod manager
	void Init();
	//Load all the mods
	void LoadAllMods();
	//Load a mod
	void LoadMod(char *,char *);
	//Get the mod index number
	unsigned long GetModIndex(char *,unsigned long);
protected:
	vector<ModInfo *> modnames;
	vector<CModule *> modlist;

	char sOblivionDir[MAX_PATH];
};

#endif