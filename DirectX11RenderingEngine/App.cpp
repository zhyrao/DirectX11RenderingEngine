#include "App.h"
#include "Box.h"
#include <memory>

App::App()
	:
	wnd(800, 600, "Tiny Renderer")
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.f, 3.1415f * 2.f);
	std::uniform_real_distribution<float> ddist(0.f, 3.1415f * 1.f);
	std::uniform_real_distribution<float> odist(0.f, 3.1415f * 0.08f);
	std::uniform_real_distribution<float> rdist(6.f, 20.f);

	for (auto i = 0; i < 180; ++i)
	{
		boxes.push_back(std::make_unique<Box>(
			wnd.Gfx(), rng, adist,
			ddist, odist, rdist
			));
	}

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.f / 4.f, .5f, 40.f));
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
App::~App()
{

}

void App::DoFrame()
{
	auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer(.07f, .0f, .12f);
	for (auto& b: boxes)
	{
		b->Update(dt);
		b->Draw(wnd.Gfx());
	}
	wnd.Gfx().EndFrame();
}
