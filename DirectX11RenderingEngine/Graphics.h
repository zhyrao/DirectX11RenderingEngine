#pragma once

#include "Win.h"
#include <d3d11.h>


class Graphics
{
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();

	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept;
private:
	IDXGISwapChain* pSwapChain = NULL;
	ID3D11Device* pDevice = NULL;
	ID3D11DeviceContext* pContext = NULL;
	ID3D11RenderTargetView* pTarget = NULL;
};

