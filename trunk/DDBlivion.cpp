// DDBlivion.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "CEngine.h"

// Global Variables:
HINSTANCE g_hInst=NULL; // current instance
HWND g_debug_hwnd=NULL; // debug window hwnd
HWND g_directx_hwnd=NULL;

void process_messages();

void output_text(WCHAR *tmpstring)
{
	unsigned long length;
	
	length=GetWindowTextLength(GetDlgItem(g_debug_hwnd,IDC_EDIT_DEBUG));
	SendDlgItemMessage(g_debug_hwnd,IDC_EDIT_DEBUG,EM_SETSEL,length,length);
	SendDlgItemMessage(g_debug_hwnd,IDC_EDIT_DEBUG,EM_REPLACESEL,0,(LPARAM)tmpstring);
	process_messages();
}

void output_text_char(WCHAR *tmpstring, char *charstring)
{
	unsigned long length;
	WCHAR temp_d[1024];
	WCHAR temp_s[512];

	memset(temp_s,0,512);
	MultiByteToWideChar(CP_ACP,0,charstring,-1,temp_s,512);

	length=GetWindowTextLength(GetDlgItem(g_debug_hwnd,IDC_EDIT_DEBUG));

	memset(temp_d,0,1024);
	swprintf_s(temp_d,1024,L"%s %s\r\n",tmpstring,temp_s);
	SendDlgItemMessage(g_debug_hwnd,IDC_EDIT_DEBUG,EM_SETSEL,length,length);
	SendDlgItemMessage(g_debug_hwnd,IDC_EDIT_DEBUG,EM_REPLACESEL,0,(LPARAM)temp_d);
}

void output_text_value(WCHAR *tmpstring,unsigned long error)
{
	unsigned long length;
	WCHAR temp_d[1024];

	memset(temp_d,0,1024);
	length=GetWindowTextLength(GetDlgItem(g_debug_hwnd,IDC_EDIT_DEBUG));

	memset(temp_d,0,1024);
	swprintf_s(temp_d,1024,L"%s %d\r\n",tmpstring,error);
	SendDlgItemMessage(g_debug_hwnd,IDC_EDIT_DEBUG,EM_SETSEL,length,length);
	SendDlgItemMessage(g_debug_hwnd,IDC_EDIT_DEBUG,EM_REPLACESEL,0,(LPARAM)temp_d);
}

void output_text_hex(WCHAR *tmpstring,unsigned long error)
{
	unsigned long length;
	WCHAR temp_d[1024];

	memset(temp_d,0,1024);
	length=GetWindowTextLength(GetDlgItem(g_debug_hwnd,IDC_EDIT_DEBUG));

	memset(temp_d,0,1024);
	swprintf_s(temp_d,1024,L"%s %X\r\n",tmpstring,error);
	SendDlgItemMessage(g_debug_hwnd,IDC_EDIT_DEBUG,EM_SETSEL,length,length);
	SendDlgItemMessage(g_debug_hwnd,IDC_EDIT_DEBUG,EM_REPLACESEL,0,(LPARAM)temp_d);
}

void process_messages()
{
	MSG msg;

	while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

void do_your_stuff()
{
	CEngine *pEngine;
	pEngine=new CEngine();
	try
	{
		pEngine->Init();
	}
	catch(WCHAR *)
	{
		output_text(L"Program exception occured...\r\n");
	}
	catch(...)
	{
		output_text(L"Unknown exception occured...\r\n");
	}
	delete pEngine;
}

LRESULT CALLBACK DlgDebugProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		g_debug_hwnd=hWndDlg;
		output_text(L"DDBlivion v0.001a\r\n");
		return TRUE;
	case WM_COMMAND:
		switch(wParam)
		{
		case IDC_BUTTON_RUN:
			do_your_stuff();
			return TRUE;
		case IDC_BUTTON_EXIT:
			EndDialog(hWndDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_hInst=hInstance;

	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_DD_DEBUG),NULL,reinterpret_cast<DLGPROC>(DlgDebugProc));
}

