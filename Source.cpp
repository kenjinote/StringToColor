#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "gdiplus")

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <gdiplus.h>
#include "resource.h"

TCHAR szClassName[] = TEXT("Window");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	static HWND hEdit;
	static HWND hStatic;
	static HBRUSH hBrush;
	static HBITMAP hBitmap;
	switch (msg)
	{
	case WM_CREATE:
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("変換"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 0, 0, 0, hWnd, (HMENU)1000, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hStatic = CreateWindowEx(0, TEXT("STATIC"), 0, WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hBitmap = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
		hBrush = CreatePatternBrush(hBitmap);
		break;
	case WM_SIZE:
		MoveWindow(hEdit, 10, 10, 256, 32, TRUE);
		MoveWindow(hButton, 10, 50, 256, 32, TRUE);
		MoveWindow(hStatic, 10, 90, 256, 256, TRUE);
		break;
	case WM_SETFOCUS:
		SetFocus(hEdit);
		break;
	case WM_CTLCOLORSTATIC:
		return (LRESULT)hBrush;
	case WM_COMMAND:
		if (LOWORD(wParam) == 1000 && HIWORD(wParam) == EN_CHANGE) {
			TCHAR szText[1024];
			GetWindowText(hEdit, szText, 1024);
			BOOL bSuccess = FALSE;
			Gdiplus::Color color = 0;
			int r = 255;
			int g = 255;
			int b = 255;
			int a = 255;
			if (szText[0] == L'#') {
				if (lstrlen(szText) == 7) {
					swscanf_s(szText, L"#%02x%02x%02x", &r, &g, &b);
					bSuccess = TRUE;
				}
				else if (lstrlen(szText) == 9) {
					swscanf_s(szText, L"#%02x%02x%02x%02x", &r, &g, &b, &a);
					bSuccess = TRUE;
				}
				if (bSuccess) {
					color = Gdiplus::Color(a, r, g, b);
				}
			}
			else {
				if (wcsncmp(szText, L"rgb(", 4) == 0) {
					swscanf_s(szText, L"rgb(%d,%d,%d)", &r, &g, &b);
					bSuccess = TRUE;
				}
				else if (wcsncmp(szText, L"rgba(", 5) == 0) {
					swscanf_s(szText, L"rgba(%d,%d,%d,%d)", &r, &g, &b, &a);
					bSuccess = TRUE;
				}
				if (bSuccess) {
					color = Gdiplus::Color(a, r, g, b);
				}
			}
			InvalidateRect(hStatic, 0, TRUE);
			if (bSuccess) {
				RECT rect;
				GetClientRect(hStatic, &rect);
				UpdateWindow(hStatic);
				Gdiplus::Graphics graphics(hStatic);
				graphics.FillRectangle(&Gdiplus::SolidBrush(color), 0, 0, rect.right, rect.bottom);
			}
		}
		break;
	case WM_DESTROY:
		DeleteObject(hBrush);
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	ULONG_PTR gdiToken;
	Gdiplus::GdiplusStartupInput gdiSI;
	Gdiplus::GdiplusStartup(&gdiToken, &gdiSI, NULL);
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("入力された色で塗りつぶす"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	Gdiplus::GdiplusShutdown(gdiToken);
	return (int)msg.wParam;
}
