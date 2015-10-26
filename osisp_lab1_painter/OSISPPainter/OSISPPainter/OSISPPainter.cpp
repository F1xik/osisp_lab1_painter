// OSISPPainter.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "OSISPPainter.h"
#include <windows.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HDC hdc, realDC, virtualDC, saveVirtualDC, saveDC, bufDC1, bufDC2;
RECT rect,screen;
int  iWidthMM, iHeightMM, iWidthPels, iHeightPels;
static CHOOSECOLOR colorBox;
static COLORREF penColor;
HPEN hPen;
static int lineWidth = 1;
OPENFILENAME ofn;
char szFile[100];
static DWORD rgbCurrent;
PRINTDLG pd;
HWND hWnd;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OSISPPAINTER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OSISPPAINTER));

	COLORREF acrCustClr[16]; // array of custom colors 

	// Initialize CHOOSECOLOR 
	ZeroMemory(&colorBox, sizeof(colorBox));
	colorBox.lStructSize = sizeof(colorBox);
	colorBox.hwndOwner = InitInstance(hInstance, nCmdShow);
	colorBox.lpCustColors = (LPDWORD)acrCustClr;
	colorBox.rgbResult = rgbCurrent;
	colorBox.Flags = CC_FULLOPEN | CC_RGBINIT;



	// Initialize PRINTDLG
	ZeroMemory(&pd, sizeof(PRINTDLG));
	pd.lStructSize = sizeof(PRINTDLG);
	pd.hwndOwner = InitInstance(hInstance, nCmdShow);
	pd.hDevMode = NULL;      	
	pd.hDevNames = NULL;    	
	pd.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
	pd.nCopies = 1;
	pd.nFromPage = 0xFFFF;
	pd.nToPage = 0xFFFF;
	pd.nMinPage = 1;
	pd.nMaxPage = 0xFFFF;

	// Main message loop:
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OSISPPAINTER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_OSISPPAINTER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   //HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return hWnd;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

COLORREF ColorDialogCreate(HWND hwnd)
{
	if (ChooseColor(&colorBox))
	{
		return colorBox.rgbResult;
	}
	return NULL;
}

LPCWSTR GetFileName(LPCSTR flag){	
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof (ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (flag == "open")
		GetOpenFileName(&ofn);
	else
	if (flag == "save")
		GetSaveFileName(&ofn);
	return (LPCWSTR)ofn.lpstrFile;
}

void SaveFile(){
	HDC hdcRef = GetDC(hWnd);
	iWidthMM = GetDeviceCaps(hdcRef, HORZSIZE);
	iHeightMM = GetDeviceCaps(hdcRef, VERTSIZE);
	iWidthPels = GetDeviceCaps(hdcRef, HORZRES);
	iHeightPels = GetDeviceCaps(hdcRef, VERTRES);

	RECT rectemf;

	screen.right = GetDeviceCaps(hdcRef, HORZRES);
	screen.bottom = GetDeviceCaps(hdcRef, VERTRES);
	GetClientRect(hWnd, &rectemf);
	screen.left = (screen.left * iWidthMM * 100) / iWidthPels;
	screen.top = (screen.top * iHeightMM * 100) / iHeightPels;
	screen.right = (screen.right * iWidthMM * 100) / iWidthPels;
	screen.bottom = (screen.bottom * iHeightMM * 100) / iHeightPels;	


	HDC hdcMeta = CreateEnhMetaFile(hdcRef, GetFileName("save"), &screen, NULL);
	GetClientRect(hWnd, &rectemf);

	
	PatBlt(hdcMeta, 0, 0, rectemf.right, rectemf.bottom + 0, PATCOPY);
	BitBlt(hdcMeta, 0, 0, rectemf.right, rectemf.bottom + 0, GetDC(hWnd), 0, 0, SRCCOPY);
	CloseEnhMetaFile(hdcMeta);
	ReleaseDC(hWnd, hdcRef);
}

void OpenFile(){
	HENHMETAFILE hemf = GetEnhMetaFile(GetFileName("open"));
	HDC hDC = GetDC(hWnd);
	bufDC1 = CreateCompatibleDC(hDC);
	bufDC2 = CreateCompatibleDC(hDC);
	GetClientRect(hWnd, &screen);
	PlayEnhMetaFile(hDC, hemf, &screen);
	PlayEnhMetaFile(bufDC1, hemf, &screen);
	BitBlt(bufDC2, 0, 0, screen.right, screen.bottom, bufDC1, 0, 0, SRCCOPY);
	DeleteEnhMetaFile(hemf);
	ReleaseDC(hWnd, hDC);
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	
	static POINT oldPoint, newPoint;	
	static int figureDrawIndex, xDelta = 0, yDelta = 0, key, delta;
	static double scaleMultiplier = 1.0;
	static BOOL isMouseDraw, isEMFwrite;
	int x, y;
	

	switch (message)
	{
	case WM_CREATE:
		ShowWindow(hWnd, SW_SHOWMAXIMIZED);
		GetClientRect(hWnd, &rect);
		realDC = GetDC(hWnd);
		virtualDC = CreateCompatibleDC(realDC);
		saveVirtualDC = CreateCompatibleDC(virtualDC);		
		FillRect(saveDC, &rect, WHITE_BRUSH);
		SelectObject(virtualDC, CreateCompatibleBitmap(realDC, rect.right, rect.bottom));
		SelectObject(saveVirtualDC, CreateCompatibleBitmap(realDC, rect.right, rect.bottom));
		FillRect(virtualDC, &rect, WHITE_BRUSH);
		FillRect(saveVirtualDC, &rect, WHITE_BRUSH);
		ReleaseDC(hWnd, realDC);
		break;
	
	
	case WM_LBUTTONDOWN:
		GetCursorPos(&oldPoint);
		ScreenToClient(hWnd, &oldPoint);
		isMouseDraw = true;
		oldPoint.x = (oldPoint.x * scaleMultiplier) + xDelta;
		oldPoint.y = (oldPoint.y * scaleMultiplier) + yDelta;
		MoveToEx(virtualDC, oldPoint.x, oldPoint.y, NULL);
		MoveToEx(saveDC, oldPoint.x, oldPoint.y, NULL);
		BitBlt(saveVirtualDC, 0, 0, rect.right, rect.bottom, virtualDC, 0, 0, SRCCOPY);
		InvalidateRect(hWnd, NULL, false);
		UpdateWindow(hWnd);
		break;
	case WM_LBUTTONUP:
		isMouseDraw = false;
		switch (figureDrawIndex)
		{
		case ID_TOOLS_PENCIL:

			break;
		case ID_TOOLS_LINE:			
			MoveToEx(saveDC, oldPoint.x, oldPoint.y, NULL);
			LineTo(saveDC, (newPoint.x * scaleMultiplier) + xDelta, (newPoint.y * scaleMultiplier) + yDelta);			
			break;
		default:
			break;
		}
		break;
		break;
	case WM_MOUSEMOVE:
		if (isMouseDraw)
		{
			switch (figureDrawIndex)
			{
			case ID_TOOLS_PENCIL:
				GetCursorPos(&newPoint);
				ScreenToClient(hWnd, &newPoint);
				x = newPoint.x * scaleMultiplier + xDelta;
				y = newPoint.y * scaleMultiplier + yDelta;				
				LineTo(virtualDC, x, y);
				LineTo(saveDC, x, y);
				oldPoint.x = x;
				oldPoint.y = y;

				InvalidateRect(hWnd, NULL, false);
				UpdateWindow(hWnd);
				break;
			case ID_TOOLS_LINE:				
				BitBlt(virtualDC, 0, 0, rect.right, rect.bottom, saveVirtualDC, 0, 0, SRCCOPY);
				GetCursorPos(&newPoint);
				ScreenToClient(hWnd, &newPoint);
				MoveToEx(virtualDC, oldPoint.x, oldPoint.y, NULL);
				LineTo(virtualDC, (newPoint.x * scaleMultiplier) + xDelta, (newPoint.y * scaleMultiplier) + yDelta);
				InvalidateRect(hWnd, NULL, false);
				UpdateWindow(hWnd);
				break;
			
			default:
				break;
			}
		}
		break;
	case WM_MOUSEWHEEL:
		key = GET_KEYSTATE_WPARAM(wParam);
		delta = GET_WHEEL_DELTA_WPARAM(wParam);
		switch (key)
		{
		case MK_CONTROL:
			(delta > 0) ? scaleMultiplier += 0.05 : scaleMultiplier -= 0.05;
			break;
		case MK_SHIFT:
			(delta > 0) ? xDelta += 20 : xDelta -= 20;
			break;
		default:
			(delta > 0) ? yDelta += 20 : yDelta -= 20;
			break;
		}
		InvalidateRect(hWnd, NULL, false);
		UpdateWindow(hWnd);
		break;
		
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_TOOLS_PENCIL:
			figureDrawIndex = ID_TOOLS_PENCIL;			
			break;
		case ID_TOOLS_LINE:
			figureDrawIndex = ID_TOOLS_LINE;			
			break;
		case ID_SETTINGS_COLOR:
			penColor = ColorDialogCreate(hWnd);
			hPen = CreatePen(PS_SOLID, lineWidth, penColor);
			DeleteObject(SelectObject(virtualDC, hPen));
			DeleteObject(SelectObject(saveDC, hPen));			
			break;
		case ID_LINEWIDTH_SMALL:
			lineWidth = 1;
			hPen = CreatePen(PS_SOLID, lineWidth, penColor);
			DeleteObject(SelectObject(virtualDC, hPen));
			DeleteObject(SelectObject(saveDC, hPen));
			break;
		case ID_LINEWIDTH_MEDIUM:
			lineWidth = 5;
			hPen = CreatePen(PS_SOLID, lineWidth, penColor);
			DeleteObject(SelectObject(virtualDC, hPen));
			DeleteObject(SelectObject(saveDC, hPen));
			break;
		case ID_LINEWIDTH_LARGE:
			lineWidth = 10;
			hPen = CreatePen(PS_SOLID, lineWidth, penColor);
			DeleteObject(SelectObject(virtualDC, hPen));
			DeleteObject(SelectObject(saveDC, hPen));
			break;
		case ID_FILE_SAVE:			
			SaveFile();
			break;
		case ID_FILE_OPEN:			
			OpenFile();
			break;
		case ID_FILE_PRINTTOFILE:
			if (PrintDlg(&pd) == TRUE)
			{
				
				DeleteDC(pd.hDC);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		realDC = BeginPaint(hWnd, &ps);
		StretchBlt(realDC, 0, 0, rect.right, rect.bottom, virtualDC, xDelta, yDelta,
			rect.right * scaleMultiplier, rect.bottom * scaleMultiplier, SRCCOPY);		
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



// Message handler for about box.
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
