#include "App.h"

int CALLBACK WinMain(
			 HINSTANCE hInstance,
		     HINSTANCE hPrevInstance,
	         LPSTR lpCmdLine,
			 int nShowCmd)
{
	try
	{
		return App{}.Go();
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