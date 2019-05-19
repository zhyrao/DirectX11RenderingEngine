#include "App.h"



App::App()
	:
	wnd(800, 600, "Tiny Renderer")
{
}

int App :: Go()
{
	MSG msg;
	BOOL gResult;

	while ((gResult = GetMessage(&msg, NULL, 0, 0)) > 0)
	{
		// Translate message 
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		DoFrame();
	}

	if (gResult == -1)
	{
		throw WND_LAST_EXCEPT();
	}

	// wParam here is the value passed to PostQuitMessage
	return msg.wParam;
}

void App::DoFrame()
{

}
