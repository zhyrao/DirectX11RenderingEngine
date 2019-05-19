#include "Graphics.h"

#pragma comment(lib, "d3d11.lib")

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// CREATE device and front/back buffers, 
	// and swap chain and rendering context
	D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		NULL,
		&pContext);

	// gain access to texture subtexture in swap chain
	ID3D11Resource* pBackBuffer = NULL;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	pDevice->CreateRenderTargetView(pBackBuffer,
		NULL,
		&pTarget);

	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	if (pTarget)
		pTarget->Release();

	if (pContext != NULL)
		pContext->Release();

	if (pSwapChain != NULL)
		pSwapChain->Release();

	if (pDevice != NULL)
		pDevice->Release();
}

void Graphics::EndFrame()
{
	if (pSwapChain != NULL)
		pSwapChain->Present(1u, 0u);
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float color[] = { r,g,b,1.f };
	pContext->ClearRenderTargetView(pTarget, color);
}
