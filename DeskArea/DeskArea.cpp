// DeskArea.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "DeskArea.h"
#include <shellapi.h>

#define MAX_LOADSTRING 100
#define	CST_DEFAULTSIZE 100

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void				InitNotifyIcon(HWND);
void				DeleteNotifyIcon(HWND);

void				ShowBar(HWND);
void				HideBar(HWND);
void				MoveBar(HWND);
void				ToggleBar(HWND);
void				SaveSetting(void);
void				LoadSetting(void);

DWORD				uEdge, nWidth, nHeight;
DWORD				nCommand;
TCHAR				szRegPath[]=TEXT("Software\\LyxSoft Software Inc.\\DeskArea");
TCHAR				szCopyRight[]=TEXT("Copyright (c) LyxSoft Software Inc. 2012\nAll rights reserved.");

//
// Main Function
//
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	// UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �ڴ˷��ô��롣

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DESKAREA, szWindowClass, MAX_LOADSTRING);

	if (lstrcmp(lpCmdLine, TEXT("SHOW")) == 0)
		nCommand = 1;
	else if (lstrcmp(lpCmdLine, TEXT("HIDE")) == 0)
		nCommand = 2;
	else if (lstrcmp(lpCmdLine, TEXT("TOGGLE")) == 0)
		nCommand = 3;
	else
		nCommand = 0;
	LoadSetting();

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
		return FALSE;

	MSG msg;
	HACCEL hAccelTable;

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DESKAREA));

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DESKAREA));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DESKAREA);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	BOOL bCreate = FALSE;

	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

	hWnd = FindWindow(szWindowClass, szTitle);
	if (!hWnd)
	{
		bCreate = TRUE;
		MyRegisterClass(hInstance);

		hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUPWINDOW | WS_CAPTION, 
		  0, 0, 0, 0, NULL, NULL, hInstance, NULL);

	}

	if (!hWnd)
		return FALSE;

	switch (nCommand)
	{
	case 0:	//Default
		if (bCreate)
			ShowBar(hWnd);
		break;
	case 1:	//Show
		ShowBar(hWnd);
		break;
	case 2:	//Hide
		HideBar(hWnd);
		break;
	case 3:	//Toggle
		ToggleBar(hWnd);
		break;
	default:
		return FALSE;
	}

	if (bCreate)
		InitNotifyIcon(hWnd);

	//ShowWindow(hWnd, nCmdShow);
	//UpdateWindow(hWnd);
	
	return bCreate;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			return 0;
		case IDM_T50:
			uEdge = ABE_TOP;
			nWidth = CST_DEFAULTSIZE;
			nHeight = 50;
			break;
		case IDM_T80:
			uEdge = ABE_TOP;
			nWidth = CST_DEFAULTSIZE;
			nHeight = 80;
			break;
		case IDM_T100:
			uEdge = ABE_TOP;
			nWidth = CST_DEFAULTSIZE;
			nHeight = 100;
			break;
		case IDM_L50:
			uEdge = ABE_LEFT;
			nWidth = 50;
			nHeight = CST_DEFAULTSIZE;
			break;
		case IDM_L80:
			uEdge = ABE_LEFT;
			nWidth = 80;
			nHeight = CST_DEFAULTSIZE;
			break;
		case IDM_L100:
			uEdge = ABE_LEFT;
			nWidth = 100;
			nHeight = CST_DEFAULTSIZE;
			break;
		case IDM_R50:
			uEdge = ABE_RIGHT;
			nWidth = 50;
			nHeight = CST_DEFAULTSIZE;
			break;
		case IDM_R80:
			uEdge = ABE_RIGHT;
			nWidth = 80;
			nHeight = CST_DEFAULTSIZE;
			break;
		case IDM_R100:
			uEdge = ABE_RIGHT;
			nWidth = 100;
			nHeight = CST_DEFAULTSIZE;
			break;
		case IDM_B50:
			uEdge = ABE_BOTTOM;
			nWidth = CST_DEFAULTSIZE;
			nHeight = 50;
			break;
		case IDM_B80:
			uEdge = ABE_BOTTOM;
			nWidth = CST_DEFAULTSIZE;
			nHeight = 80;
			break;
		case IDM_B100:
			uEdge = ABE_BOTTOM;
			nWidth = CST_DEFAULTSIZE;
			nHeight = 100;
			break;
		case IDM_SHOW:
			ShowBar (hWnd);
			MoveBar(hWnd);
			return 0;
		case IDM_HIDE:
			HideBar (hWnd);
			return 0;
		case IDM_ABOUT:
			ShellAbout (hWnd, szTitle, szCopyRight, (HICON)GetClassLong (hWnd, GCL_HICON));
			/*case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;*/
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		MoveBar (hWnd);
		SaveSetting();
		break;

	case WM_DESTROY:
		DeleteNotifyIcon (hWnd);
		HideBar (hWnd);

		PostQuitMessage(0);
		break;
	case WM_USER:
    	if (lParam == WM_LBUTTONUP || lParam == WM_RBUTTONUP)
        {
			HMENU hMenu;
			HMENU hPopup;
            POINT	CursorPos;

            GetCursorPos (&CursorPos);

			hMenu = LoadMenu (hInst, MAKEINTRESOURCE(IDC_DESKAREA));
			if (hMenu == NULL)
				return 0;
			hPopup = GetSubMenu (hMenu, 0);
			TrackPopupMenu(hPopup, TPM_LEFTALIGN | TPM_LEFTBUTTON, CursorPos.x, CursorPos.y, 0, hWnd, NULL); 
			DestroyMenu (hMenu);
        }
        break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/*
// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
*/

void ShowBar (HWND hWnd)
{
	APPBARDATA cBar;

	cBar.cbSize = sizeof cBar;
	cBar.hWnd = hWnd;
	cBar.uCallbackMessage = WM_MOUSEMOVE;
	
	if ((BOOL)SHAppBarMessage (ABM_NEW, &cBar))
		MoveBar (hWnd);
}

void MoveBar (HWND hWnd)
{
	int	nScreenX;
	int	nScreenY;
	APPBARDATA cBar;

	nScreenX = GetSystemMetrics (SM_CXSCREEN);
	nScreenY = GetSystemMetrics (SM_CYSCREEN);

	cBar.cbSize = sizeof cBar;
	cBar.hWnd = hWnd;
	cBar.uCallbackMessage = WM_MOUSEMOVE;

	switch (uEdge)
	{
	case ABE_LEFT:
		cBar.rc.left = 0;
		cBar.rc.top = 0;
		cBar.rc.right = nWidth;
		cBar.rc.bottom = CST_DEFAULTSIZE;
		break;
	case ABE_TOP:
		cBar.rc.left = 0;
		cBar.rc.top = 0;
		cBar.rc.right = CST_DEFAULTSIZE;
		cBar.rc.bottom = nHeight;
		break;
	case ABE_BOTTOM:
		cBar.rc.left = 0;
		cBar.rc.top = nScreenY - nHeight;
		cBar.rc.bottom = nScreenY;
		cBar.rc.right = CST_DEFAULTSIZE;
		break;

	default:	//ABE_RIGHT, default
		uEdge = ABE_RIGHT;
		cBar.rc.left = nScreenX - nWidth;
		cBar.rc.top = 0;
		cBar.rc.right = nScreenX;
		cBar.rc.bottom = CST_DEFAULTSIZE;
		break;
	}
	cBar.uEdge = uEdge;

	SHAppBarMessage(ABM_QUERYPOS, &cBar);
	SHAppBarMessage (ABM_SETPOS, &cBar);
}

void HideBar (HWND hWnd)
{
	APPBARDATA cBar;

	cBar.cbSize = sizeof cBar;
	cBar.hWnd = hWnd;

	SHAppBarMessage (ABM_REMOVE, &cBar);
}

void ToggleBar(HWND hWnd)
{
	APPBARDATA cBar;

	cBar.cbSize = sizeof cBar;
	cBar.hWnd = hWnd;
	cBar.uCallbackMessage = WM_MOUSEMOVE;

	if ((BOOL)SHAppBarMessage(ABM_NEW, &cBar))	//Return false if already registered
		MoveBar(hWnd);
	else
		SHAppBarMessage(ABM_REMOVE, &cBar);
}

void InitNotifyIcon (HWND hWnd)
{
	NOTIFYICONDATA	cNotifyIconData;

    cNotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
    cNotifyIconData.hWnd = hWnd;
    cNotifyIconData.uID = 1;
    cNotifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    cNotifyIconData.uCallbackMessage = WM_USER;
    cNotifyIconData.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));
    lstrcpy (cNotifyIconData.szTip, szTitle);
	Shell_NotifyIcon (NIM_ADD, &cNotifyIconData);
}

void DeleteNotifyIcon (HWND hWnd)
{
	NOTIFYICONDATA	cNotifyIconData;

	cNotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	cNotifyIconData.hWnd = hWnd;
	cNotifyIconData.uID = 1;

	Shell_NotifyIcon (NIM_DELETE, &cNotifyIconData);
}

void SaveSetting (void)
{
	HKEY hRegKey;

	if (ERROR_SUCCESS == RegCreateKeyEx (HKEY_CURRENT_USER, szRegPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hRegKey, NULL))
	{
		RegSetValueEx (hRegKey, TEXT("uEdge"), NULL, REG_DWORD, (LPBYTE)&uEdge, sizeof DWORD);
		RegSetValueEx (hRegKey, TEXT("Width"), NULL, REG_DWORD, (LPBYTE)&nWidth, sizeof DWORD);
		RegSetValueEx (hRegKey, TEXT("Height"), NULL, REG_DWORD, (LPBYTE)&nHeight, sizeof DWORD);
	}
}

void LoadSetting(void)
{
	HKEY hRegKey;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, szRegPath, 0, KEY_READ, &hRegKey))
	{
		DWORD	nSize = sizeof(DWORD);

		if (ERROR_SUCCESS != RegQueryValueEx(hRegKey, TEXT("uEdge"), NULL, NULL, (LPBYTE)&(uEdge), &nSize))
			uEdge = ABE_LEFT;

		if (ERROR_SUCCESS != RegQueryValueEx(hRegKey, TEXT("Width"), NULL, NULL, (LPBYTE)&nWidth, &nSize))
			nWidth = CST_DEFAULTSIZE;

		if (ERROR_SUCCESS != RegQueryValueEx(hRegKey, TEXT("Height"), NULL, NULL, (LPBYTE)&nHeight, &nSize))
			nHeight = CST_DEFAULTSIZE;
		RegCloseKey(hRegKey);
	}
}