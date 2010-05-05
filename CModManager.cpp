#include "stdafx.h"
#include "Global.h"
#include "CModManager.h"
#include "CTreeManager.h"

CModule::CModule()
{
	main_header=NULL;
}

CModule::~CModule()
{
	if(main_header) delete main_header;
}

void CModule::LoadMod(HANDLE mod_handle, CModManager *mod_handler,char *mod_name)
{
	DWORD readen;
	unsigned long filesize,headersize;
	unsigned char *buf;

	//Allocate buffer for the file
	filesize=GetFileSize(mod_handle,NULL);
	buf=new unsigned char[filesize];

	//Read the file
	SetFilePointer(mod_handle,0,NULL,FILE_BEGIN);
	ReadFile(mod_handle,buf,filesize,&readen,NULL);

	//Add a check for ReadFile return value maybe
	if(filesize!=readen)
	{
		delete [] buf;
		throw L"Error, filesize!=readen";
	}

	//Read the TES4 to get the master list
	headersize=LoadHeader(buf,filesize);
	//Add the mod to its own masterlist
	masterlist.push_back(mod_name);
	//Process the masterlist
	BuildFormidTranslation(mod_handler);


	pTreeManager->LoadTreeRawGrup(&buf[headersize],filesize-headersize,NULL,this);

	delete [] buf;
}

unsigned long CModule::LoadHeader(unsigned char *pointer,unsigned long filesize)
{
	unsigned long curpointer;
	char *master_filename;
	RecordHeader tes4header;
	SubRecordHeader subrecord;

	curpointer=0;
	//Read the header
	memcpy(&tes4header,pointer,sizeof(RecordHeader));
	curpointer+=sizeof(RecordHeader);

	//Read the subrecords
	while(curpointer<tes4header.size)
	{
		memcpy(&subrecord,&pointer[curpointer],sizeof(SubRecordHeader));
		if(strncmp(subrecord.type,"MAST",4)==0)
		{
			master_filename=new char[MAX_PATH];
			//Get the string
			memset(master_filename,0,MAX_PATH);
			strcpy_s(master_filename,MAX_PATH,(char *)&pointer[6+curpointer]);
			//Add to the local master list
			masterlist.push_back(master_filename);
		}
		curpointer+=(6+subrecord.size);
	}
	return(sizeof(RecordHeader)+tes4header.size);
}

void CModule::BuildFormidTranslation(CModManager *mod_handler)
{
	unsigned long index;
	for(index=0;index<masterlist.size();index++)
	{
		if(index!=(masterlist.size()-1)) local_to_global_formid.push_back(mod_handler->GetModIndex(masterlist[index],1));
		else local_to_global_formid.push_back(mod_handler->GetModIndex(masterlist[index],0));
	}
}

unsigned long CModule::TranslateFormID(unsigned long formID)
{
	if((formID>>24)==0) return formID;
	if((formID>>24)>=local_to_global_formid.size())
	{
		//output_text_hex(L"Possible invalid formID:",formID);
		return((formID&0x00FFFFFF)|(local_to_global_formid[local_to_global_formid.size()-1]<<24));
		//throw L"Error formID, out of range formID!";
	}

	return((formID&0x00FFFFFF)|(local_to_global_formid[(formID>>24)]<<24));
}

CModManager::CModManager()
{
}

CModManager::~CModManager()
{
	unsigned long index;
	for(index=0;index<modlist.size();index++)
	{
		delete modlist[index];
	}
	modlist.clear();
}

void CModManager::Init()
{
	char sPathPluginsTxt[MAX_PATH];
	unsigned long index,index2;
	unsigned char *bPluginsTxt;
	unsigned long iPluginsTxtSize;
	unsigned long readen;
	ModInfo *currentmod;

	HKEY resultkey;
	HANDLE hPluginsTxt;
	DWORD keyType=REG_SZ;
	DWORD keySize=MAX_PATH;

	//Get oblivion installation path
#ifdef WIN64_BUILD
	if(RegOpenKeyExA(HKEY_LOCAL_MACHINE,"SOFTWARE\\WOW6432Node\\Bethesda Softworks\\Oblivion\\",0,KEY_READ,&resultkey)!=ERROR_SUCCESS) throw L"Couldn't open/find the key associated with Oblivion in the registry...\r\n";
#else
	if(RegOpenKeyExA(HKEY_LOCAL_MACHINE,"SOFTWARE\\Bethesda Softworks\\Oblivion\\",0,KEY_READ,&resultkey)!=ERROR_SUCCESS) throw L"Couldn't open/find the key associated with Oblivion in the registry...\r\n";
#endif
	if(RegQueryValueExA(resultkey,"Installed Path",0,&keyType,(LPBYTE)sOblivionDir,&keySize)!=ERROR_SUCCESS) throw L"Couldn't read the Installed Path in registry...\r\n";
	RegCloseKey(resultkey);

	strcat_s(sOblivionDir,MAX_PATH,"\\data\\");

	//Get the directory for the plugins.txt
	if(SHGetFolderPathA(NULL,CSIDL_LOCAL_APPDATA,NULL,SHGFP_TYPE_CURRENT,sPathPluginsTxt)!=S_OK) throw L"Error getting appdata path...";

	strcat_s(sPathPluginsTxt,MAX_PATH,"\\Oblivion\\Plugins.txt");
	hPluginsTxt=CreateFileA(sPathPluginsTxt,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
	if(hPluginsTxt==INVALID_HANDLE_VALUE) throw L"Can't open plugins.txt to see the list of active plugins...\r\n";
	
	iPluginsTxtSize=GetFileSize(hPluginsTxt,NULL);
	bPluginsTxt=new unsigned char[iPluginsTxtSize];
	SetFilePointer(hPluginsTxt,0,NULL,FILE_BEGIN);
	ReadFile(hPluginsTxt,bPluginsTxt,iPluginsTxtSize,&readen,NULL);
	CloseHandle(hPluginsTxt);

	//Parse the plugins.txt to get the list of active plugins
	currentmod=new ModInfo;
	memset(currentmod->path,0,MAX_PATH);
	index2=0;
	for(index=0;index<iPluginsTxtSize;index++)
	{
		if(bPluginsTxt[index]=='\r'||bPluginsTxt[index]=='\n')
		{
			if(index2>4&&currentmod->path[0]!='#')
			{
				modnames.push_back(currentmod);
				currentmod=new ModInfo;
			}
			else
			{
				memset(currentmod->path,0,MAX_PATH);
			}
			index2=0;
		}
		else
		{
			currentmod->path[index2]=bPluginsTxt[index];
			index2++;
		}
	}

	//In case there is a mod with no rn at the end
	if(index2>4&&currentmod->path[0]!='#')
	{
		modnames.push_back(currentmod);
		currentmod=new ModInfo;
	}

	delete [] bPluginsTxt;

	for(index=0;index<modnames.size();index++)
	{
		if(_stricmp(&modnames[index]->path[strlen(modnames[index]->path)-4],".esm")==0)
		{
			modnames[index]->type=0;
		}
		else
		{
			if(_stricmp(&modnames[index]->path[strlen(modnames[index]->path)-4],".esp")==0)
			{
				modnames[index]->type=1;
			}
			else
			{
				throw L"Unknown mod type(not .esm/.esp)!";
			}
		}
	}

	//Reorder the mods(by modified date/time
	char fullmodpath[MAX_PATH];
	WIN32_FILE_ATTRIBUTE_DATA modtimeinfo;

	for(index=0;index<modnames.size();index++)
	{
		memset(fullmodpath,0,MAX_PATH);
		strcpy_s(fullmodpath,MAX_PATH,sOblivionDir);
		strcat_s(fullmodpath,MAX_PATH,modnames[index]->path);
		if(GetFileAttributesExA(fullmodpath,GetFileExInfoStandard,&modtimeinfo)==0)
		{
			output_text_char(L"Couldn't get the timestamp of",fullmodpath);
			throw L"Error timestamp!";
		}
		modnames[index]->timestamp.dwHighDateTime=modtimeinfo.ftLastWriteTime.dwHighDateTime;
		modnames[index]->timestamp.dwLowDateTime=modtimeinfo.ftLastWriteTime.dwLowDateTime;
		modnames[index]->loaded=0;
	}

	unsigned long numchange=0;
	ModInfo *tempinfos;
	unsigned long swapped=1;

	while(swapped)
	{
		swapped=0;
		for(index=0;index<(modnames.size()-1);index++)
		{
			if(modnames[index]->type==modnames[index+1]->type)
			{
				if(modnames[index]->timestamp.dwHighDateTime==modnames[index+1]->timestamp.dwHighDateTime)
				{
					//check low date time
					if(modnames[index]->timestamp.dwLowDateTime==modnames[index+1]->timestamp.dwLowDateTime)
					{
						output_text(L"Two mods have the exact same timestamp...\r\n");
						output_text(L"Time to use OBMM/Wrye Bash/BOSS!\r\n");
						throw L"Error identical timestamps!\r\n";
					}
					if(modnames[index]->timestamp.dwLowDateTime>modnames[index+1]->timestamp.dwLowDateTime)
					{
						swapped=1;
						numchange++;
						//exchange the 2
						tempinfos=modnames[index];
						modnames[index]=modnames[index+1];
						modnames[index+1]=tempinfos;
					}
				}
				else
				{
					if(modnames[index]->timestamp.dwHighDateTime>modnames[index+1]->timestamp.dwHighDateTime)
					{
						swapped=1;
						numchange++;
						//exchange the 2
						tempinfos=modnames[index];
						modnames[index]=modnames[index+1];
						modnames[index+1]=tempinfos;
					}
				}
			}
			else
			{
				//One is an esm and the other an .esp
				if(modnames[index]->type==1)
				{
					swapped=1;
					numchange++;
					//exchange the 2
					tempinfos=modnames[index];
					modnames[index]=modnames[index+1];
					modnames[index+1]=tempinfos;
				}
			}
		}
	}

	if(numchange==0)
	{
		output_text(L"Ah, a plugins.txt already sorted, I like that!\r\n");
	}
}

void CModManager::LoadMod(char *name,char *short_name)
{
	HANDLE mod_handle;
	mod_handle=CreateFileA(name,GENERIC_READ,NULL,NULL,OPEN_EXISTING,NULL,NULL);
	if(mod_handle==INVALID_HANDLE_VALUE) throw L"Error opening a mod!";

	CModule *newmod;
	newmod=new CModule();
	
	newmod->LoadMod(mod_handle,this,short_name);

	CloseHandle(mod_handle);
	modlist.push_back(newmod);
}

void CModManager::LoadAllMods()
{
	unsigned long index;
	char fullpath[MAX_PATH];

	//for(index=0;index<modnames.size();index++)
	for(index=0;index<1;index++)
	{
		output_text_char(L"Loading",modnames[index]->path);

		memset(fullpath,0,MAX_PATH);
		strcpy_s(fullpath,MAX_PATH,sOblivionDir);
		strcat_s(fullpath,MAX_PATH,modnames[index]->path);

		LoadMod(fullpath,modnames[index]->path);
		modnames[index]->loaded=1;

		process_messages();
	}
}

unsigned long CModManager::GetModIndex(char *filename,unsigned long tocheck)
{
	unsigned long index;

	for(index=0;index<modnames.size();index++)
	{
		if(_stricmp(modnames[index]->path,filename)==0)
		{
			if(tocheck==1&&modnames[index]->loaded==0)
			{
				output_text_char(L"MasterFile not loaded before the mod:",filename);
				output_text(L"Please configure your load order correctly before using this tool!\r\n");
				throw L"MasterFile not loaded error!";
			}
			return index;
		}
	}
	throw L"Couldn't find the master in the mod list, critical failure!";
	return 0;
}