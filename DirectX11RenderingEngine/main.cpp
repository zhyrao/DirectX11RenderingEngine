#include "Window.h"


int CALLBACK WinMain(
			_In_ HINSTANCE hInstance,
		    _In_opt_ HINSTANCE hPrevInstance,
	        _In_ LPSTR lpCmdLine,
			_In_ int nShowCmd)
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
			return -1;
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