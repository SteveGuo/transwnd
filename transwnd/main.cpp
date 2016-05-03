/****************************************************************************
The MIT License (MIT)

Copyright (c) 2016 Steve Guo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
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
