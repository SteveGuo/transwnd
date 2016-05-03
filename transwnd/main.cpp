//
//  main.cpp
//  transwnd
//
//  Created by SteveGuo on 4/28/16.
//  Copyright (c) 2016 SteveGuo. All rights reserved.
//

#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <Psapi.h>
#include <iostream>


HWND g_hWnd = NULL;
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	DWORD dwProcessId;
	GetWindowThreadProcessId(hWnd, &dwProcessId);

	if (dwProcessId == lParam)
	{
		g_hWnd = hWnd;
		return FALSE;
	}

	return TRUE;
}

void centerWindow(HWND hWnd)
{
	RECT r0, r1;
	GetClientRect(hWnd, &r0);
	GetWindowRect(hWnd, &r1);
	SIZE c = { r0.right, r0.bottom };
	SIZE w = { r1.right - r1.left , r1.bottom - r1.top };
	auto verticalFrame = (w.cx - c.cx) / 2;
	auto topFrame = w.cy - c.cy - verticalFrame;

	SIZE s;
	s.cx = GetSystemMetrics(SM_CXFULLSCREEN);
	s.cy = GetSystemMetrics(SM_CYFULLSCREEN);

	POINT pt = { 0 };
	pt.x = (s.cx - c.cx) / 2;
	pt.y = (s.cy - c.cy) / 2 - topFrame;
	if (pt.y < 0) pt.y = 0;

	MoveWindow(hWnd, pt.x, pt.y, w.cx, w.cy, FALSE);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	int argc;
	LPTSTR *argv = CommandLineToArgvW(pCmdLine, &argc);

	TCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	auto length = wcslen(path);
	if (!wmemcmp(argv[0], path, length))
	{
		MessageBox(NULL, TEXT("This app is designed to start another app."), TEXT("Error"), MB_OK);
		return 1;
	}

	STARTUPINFO si = { 0 };
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION pi = { 0 };

	if (!CreateProcess(argv[0], NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi))
	{
		return 2;
	}

	while (!g_hWnd)
	{
		EnumWindows(EnumWindowsProc, pi.dwProcessId);
	}

	auto hWnd = g_hWnd;
	centerWindow(hWnd);
	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

	float percentage = 1;
	if (argc >= 2)
	{
		swscanf(argv[1], L"%f", &percentage);
	}
	SetLayeredWindowAttributes(hWnd, 0, static_cast<BYTE>(255 * percentage), LWA_ALPHA);
	SetForegroundWindow(hWnd);
	ShowWindow(hWnd, SW_SHOW);

	return 0;
}
