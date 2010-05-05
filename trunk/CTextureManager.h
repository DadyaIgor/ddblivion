#ifndef CLASSTEXTUREMANAGER
#define CLASSTEXTUREMANAGER

class CTexture
{
public:
	CTexture();
	~CTexture();
public:
	unsigned long LoadLTEX(unsigned long);
	unsigned long LoadDirect(char *);

	unsigned long Get_FormID();
	char *Get_Filename();

	ID3D10ShaderResourceView *Get_Texture();
protected:
	unsigned long formID;
	ID3D10ShaderResourceView *pTexture;
	char *filename;
};	

class CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();

	CTexture *Load_LTEX(unsigned long);
	CTexture *Load_Direct(char *);
	CTexture *GetDefaultTexture();
protected:
	vector<CTexture *> texture_list;
	vector<CTexture *> texture2_list;
};

#endif