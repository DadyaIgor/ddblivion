#ifndef GLOBAL_HEADER
#define GLOBAL_HEADER

#include "CDirectXManager.h"
#include "CInputManager.h"
#include "CTreeManager.h"
#include "CTextureManager.h"
#include "CMeshManager.h"
#include "CHavokManager.h"
#include "CWeatherManager.h"
#include "CGlobal.h"

void output_text(WCHAR *);
void output_text_value(WCHAR *,unsigned long);
void output_text_hex(WCHAR *,unsigned long);
void output_text_char(WCHAR *, char *);
void process_messages();

extern CDirectXManager *pDXManager;
extern CTree_Manager *pTreeManager;
extern CTextureManager *pTextureManager;
extern CInputManager *pInputManager;
extern CMeshManager *pMeshManager;
extern CHavokManager *pHavokManager;
extern CWeatherManager *pWeatherManager;
extern CGlobal *pGlobal;

#define ENDIANPLAY(a) (((a&0xFF)<<24)|((a&0xFF00)<<8)|((a&0xFF0000)>>8)|((a&0xFF000000)>>24))
#define CLEAN_DELETE(a) if(a) { delete a; a=NULL; }

#define SCALE_HAVOK 70.0f

#endif