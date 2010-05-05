#include "stdafx.h"
#include "DDblivion.h"
#include "CInputManager.h"

CInputManager::CInputManager()
{
	pDI=NULL;
	pDI_Keyboard=NULL;
	pDI_Mouse=NULL;
	mouseX=0;
	mouseY=0;
}

CInputManager::~CInputManager()
{
	if(pDI_Keyboard)
	{
		pDI_Keyboard->Unacquire();
		pDI_Keyboard->Release();
		pDI_Keyboard=NULL;
	}
	if(pDI_Mouse)
	{
		pDI_Mouse->Unacquire();
		pDI_Mouse->Release();
		pDI_Mouse=NULL;
	}
	if(pDI)
	{
		pDI->Release();
		pDI=NULL;
	}
}

void CInputManager::Init(HWND hwnd_dx)
{
	HRESULT result;

	result=DirectInput8Create(g_hInst,DIRECTINPUT_VERSION,IID_IDirectInput8, (void**)&pDI, NULL);
	if(FAILED(result)) throw L"Error DirectInput8Create";

	result=pDI->CreateDevice(GUID_SysKeyboard, &pDI_Keyboard, NULL);
	if(FAILED(result)) throw L"Error CreateDevice Keyboard";

	result=pDI->CreateDevice(GUID_SysMouse, &pDI_Mouse, NULL);
	if(FAILED(result)) throw L"Error CreateDevice Mouse";

	result=pDI_Keyboard->SetDataFormat(&c_dfDIKeyboard); 
	if(FAILED(result)) throw L"Error SetDataFormat Keyboard";

	result=pDI_Keyboard->SetCooperativeLevel(hwnd_dx,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result)) throw L"Error SetCooperativeLevel Keyboard";

	result=pDI_Mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result)) throw L"Error SetDataFormat Mouse";

	result=pDI_Mouse->SetCooperativeLevel(hwnd_dx,DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	if(FAILED(result)) throw L"Error SetCooperativeLevel Mouse";

	DIPROPDWORD dipdw;
	dipdw.diph.dwSize=sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize=sizeof(DIPROPHEADER);
	dipdw.diph.dwObj=0;
	dipdw.diph.dwHow=DIPH_DEVICE;
	dipdw.dwData=16;

	result=pDI_Mouse->SetProperty(DIPROP_BUFFERSIZE,&dipdw.diph);
	if(FAILED(result)) throw L"Error SetProperty Mouse";


	//Set them both to acquire
	pDI_Mouse->Acquire();
	pDI_Keyboard->Acquire();
}

void CInputManager::UpdateInput()
{
	unsigned long doloop;
	HRESULT result;
	DWORD numElements;
	DIDEVICEOBJECTDATA mouse_data;
	
	//Update Keyboard State
	result=pDI_Keyboard->GetDeviceState(256,(LPVOID)&bKeyboard);
	if(FAILED(result))
	{
		if(result==DIERR_INPUTLOST)
		{
			memset(bKeyboard,0,256);
			pDI_Keyboard->Acquire();
		}
		else
		{
			memset(bKeyboard,0,256);
			pDI_Keyboard->Acquire();
		}

		//else throw L"Unknown GetDeviceState Exception";
	}

	//Update Mouse State
	doloop=1;
	while(doloop)
	{
		numElements=1;
		result=pDI_Mouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),&mouse_data,&numElements,0);
		if(FAILED(result)||numElements==0)
		{
			if(result==DIERR_INPUTLOST) pDI_Mouse->Acquire();
			else pDI_Mouse->Acquire();
			doloop=0;
		}
		else
		{
			switch(mouse_data.dwOfs)
			{
			case DIMOFS_X:
				mouseX=mouse_data.dwData;
				break;
			case DIMOFS_Y:
				mouseY=mouse_data.dwData;
				break;
			}
		}
	}
}

unsigned long CInputManager::IsKeyPressed(unsigned char key)
{
	if(bKeyboard[key]&0x80) return 1;
	return 0;
}

void CInputManager::Get_MouseXY(long &_mouseX,long &_mouseY)
{
	_mouseX=mouseX;
	_mouseY=mouseY;

	mouseX=0;
	mouseY=0;
}