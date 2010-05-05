#include "stdafx.h"
#include "Global.h"
#include "CTreeManager.h"
#include "CTextureManager.h"

CTexture::CTexture()
{
	filename=NULL;
	formID=0;
	pTexture=NULL;
}

CTexture::~CTexture()
{
}

unsigned long CTexture::LoadLTEX(unsigned long wantID)
{
	WCHAR tmpnamebuf[MAX_PATH];
	WCHAR tmpfilenamebuf[MAX_PATH];
	CLtex_Record *datext;
	
	formID=wantID;

	datext=pTreeManager->GetLandscapeTexture(wantID);
	if(datext==NULL)
	{
		output_text_value(L"Failed to find texture ID:",wantID);
		return 2;
	}

	wcscpy_s(tmpnamebuf,MAX_PATH,L"data\\textures\\landscape\\");
	MultiByteToWideChar(CP_ACP,0,datext->Get_Filename(),-1,tmpfilenamebuf,MAX_PATH);
	wcscat_s(tmpnamebuf,MAX_PATH,tmpfilenamebuf);

	pTexture=pDXManager->CreateTexture(tmpnamebuf);
	if(pTexture==NULL)
	{
		output_text(L"Error pDXManager->CreateTexture!\r\n");
		return 1;
	}
	return 0;
}

unsigned long CTexture::LoadDirect(char *texturefilename)
{
	WCHAR tmpnamebuf[MAX_PATH];
	WCHAR tmpfilenamebuf[MAX_PATH];

	filename=new char[strlen(texturefilename)+1];
	strcpy_s(filename,strlen(texturefilename)+1,texturefilename);
	
	wcscpy_s(tmpnamebuf,MAX_PATH,L"data\\");
	MultiByteToWideChar(CP_ACP,0,texturefilename,-1,tmpfilenamebuf,MAX_PATH);
	wcscat_s(tmpnamebuf,MAX_PATH,tmpfilenamebuf);

	pTexture=pDXManager->CreateTexture(tmpnamebuf);
	if(pTexture==NULL)
	{
		output_text(L"Error pDXManager->CreateTexture!\r\n");
		return 1;
	}
	return 0;
}

unsigned long CTexture::Get_FormID()
{
	return formID;
}

char *CTexture::Get_Filename()
{
	return filename;
}

ID3D10ShaderResourceView *CTexture::Get_Texture()
{
	return pTexture;
}

CTextureManager::CTextureManager()
{
	CTexture *datext;
	datext=new CTexture();
	if(datext->LoadLTEX(0x2749E)!=0)
	{
		output_text(L"Error loading default Texture!\r\n");
		delete datext;
		return;
	}
	texture_list.push_back(datext);
}

CTextureManager::~CTextureManager()
{
	unsigned long index;
	for(index=0;index<texture_list.size();index++)
	{
		delete texture_list[index];
	}
	texture_list.clear();
}

CTexture *CTextureManager::GetDefaultTexture()
{
	return texture_list[0];
}

CTexture *CTextureManager::Load_LTEX(unsigned long wantID)
{
	unsigned long index;
	CTexture *datext;

	//Check if the texture is already loaded...
	for(index=0;index<texture_list.size();index++)
	{
		if(texture_list[index]->Get_FormID()==wantID)
		{
			return texture_list[index];
		}
	}

	datext=new CTexture();
	if(datext->LoadLTEX(wantID)!=0)
	{
		output_text(L"Failed CTextureManager::Load_LTEX\r\n");
		delete datext;
		return GetDefaultTexture();
	}
	texture_list.push_back(datext);
	return datext;
}

CTexture *CTextureManager::Load_Direct(char *texturefilename)
{
	unsigned long index;
	CTexture *datext;

	//Check if it exists
	for(index=0;index<texture2_list.size();index++)
	{
		if(_stricmp(texture2_list[index]->Get_Filename(),texturefilename)==0)
		{
			return texture2_list[index];
		}
	}

	datext=new CTexture();
	if(datext->LoadDirect(texturefilename)!=0)
	{
		output_text(L"Failed CTextureManager::Load_Direct\r\n");
		delete datext;
		return GetDefaultTexture();
	}
	texture2_list.push_back(datext);
	return datext;
}