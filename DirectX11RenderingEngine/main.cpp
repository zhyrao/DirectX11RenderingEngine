#include "Window.h"


int CALLBACK WinMain(
			_In_ HINSTANCE hInstance,
		    _In_opt_ HINSTANCE hPrevInstance,
	        _In_ LPSTR lpCmdLine,
			_In_ int nShowCmd)
{
	Window wnd(800, 600, "Tiny Engine");

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

	return msg.wParam;
}