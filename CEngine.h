#ifndef CLASSENGINE
#define CLASSENGINE

#include "CTreeManager.h"
#include "CModManager.h"
#include "CLandManager.h"
#include "CDirectXManager.h"

class CEngine
{
public:
	CEngine();
	~CEngine();
	void Init();
protected:
	CModManager *pModManager;
	CLandManager *pLandManager;
	
};

#endif