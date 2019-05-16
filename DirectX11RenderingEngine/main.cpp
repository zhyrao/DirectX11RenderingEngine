#include <Windows.h>
#include "WindowsMessageMap.h"
#include <sstream>
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static WindowsMessageMap mm;
	OutputDebugString(mm(msg, lParam, wParam).c_str());

	static std::string title;
	std::ostringstream oss;
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	case WM_KEYDOWN:
		if (wParam == 'F')
		{
			SetWindowTextA(hWnd, "Rest Window Title");
		}
		break;
	case WM_KEYUP:
		if (wParam == 'F')
		{
			SetWindowTextA(hWnd, "DirectX 11 Rendering Engine");
		}
		break;
	case WM_CHAR:
		title.push_back((char)wParam);
		SetWindowTextA(hWnd, title.c_str());

		break;
	case WM_LBUTTONDOWN:
		const POINTS pt = MAKEPOINTS(lParam);
		oss << "(" << pt.x << "," << pt.y << ")";
		SetWindowTextA(hWnd, oss.str().c_str());
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance,
		    _In_opt_ HINSTANCE hPrevInstance,
	        _In_ LPSTR lpCmdLine,
			_In_ int nShowCmd)
{

	const auto pClassName = "d3d11RenderingEngine";
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = pClassName;
	wc.hIconSm = NULL;

	// register window class
	RegisterClassEx(&wc);

	// create window
	HWND hWnd = CreateWindowExA(
		0,
		pClassName,
		"DirectX 11 Rendering Engine",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200,
		200,
		640,
		480,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, SW_SHOW);

	// message pump
	MSG msg;
	BOOL gResult;
	while ((gResult = GetMessage(&msg, NULL, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (gResult == -1)
	{
		return -1;
	}
	else
	{
		return msg.wParam;
	}
	return 0;
}