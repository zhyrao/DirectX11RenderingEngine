#include "App.h"

App::App()
	:
	wnd(800, 600, "Tiny Renderer")
{
}

int App :: Go()
{
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(c, c, 1);

	wnd.Gfx().DrawTriangle(-timer.Peek(),
		0,
		0);

	wnd.Gfx().DrawTriangle(timer.Peek(),
		wnd.mouse.GetPosX() / 400.f - 1.f,
		1.f - wnd.mouse.GetPosY() / 300.f) ;
	wnd.Gfx().EndFrame();
}
