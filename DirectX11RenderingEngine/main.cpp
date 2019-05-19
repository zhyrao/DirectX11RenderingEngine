#include "Window.h"
#include <sstream>

int CALLBACK WinMain(
			 HINSTANCE hInstance,
		     HINSTANCE hPrevInstance,
	         LPSTR lpCmdLine,
			 int nShowCmd)
{
	try
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
			throw WND_LAST_EXCEPT();
		}
	}
	catch (const ExceptionClass& e)
	{
		MessageBox(NULL, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(NULL, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(NULL, "No details available", "Unkown Exeption", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}