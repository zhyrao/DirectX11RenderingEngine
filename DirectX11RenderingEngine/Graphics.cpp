#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
#include <cmath>
#include <DirectXMath.h>
#include "GraphicsThrowMacros.h"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

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

	UINT swapCreateFlags = 0;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // !NDEBUG


	HRESULT hr;

	// CREATE device and front/back buffers, 
	// and swap chain and rendering context
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		swapCreateFlags,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		NULL,
		&pContext
	));

	// gain access to texture subtexture in swap chain
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(),NULL,&pTarget));

	// create depth stencil desc
	D3D11_DEPTH_STENCIL_DESC dsdsc = {};
	dsdsc.DepthEnable = TRUE;
	dsdsc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsdsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsdsc.StencilEnable = FALSE;
	
	wrl::ComPtr<ID3D11DepthStencilState> pDSState;
	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&dsdsc, &pDSState));

	// bind depth state
	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	// create depth stencil texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = 800u;
	descDepth.Height = 600u;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencil));

	// create view of depth stencil state view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &pDSV));

	// bind depth stencil view to OM
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800.f;
	vp.Height = 600.f;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	pContext->RSSetViewports(1u, &vp);
}

void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif // !NDEBUG

	if (FAILED(hr = pSwapChain->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
		
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float color[] = { .2f, .2f, .2f,1.f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1, 0u);
}

void Graphics::DrawIndexed(UINT count) noexcept(!IS_DEBUG)
{
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::SetProjection(DirectX::XMMATRIX proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}

//void Graphics::DrawTriangle(float angle, float x, float z)
//{
//	namespace wrl = Microsoft::WRL;
//	HRESULT hr;
//
//	struct Vertex
//	{
//		struct 
//		{
//			float x;
//			float y;
//			float z;
//		}pos;
//		struct {
//			unsigned char r;
//			unsigned char g;
//			unsigned char b;
//			unsigned char a;
//		}color;
//	};
//
//	const Vertex vertices[] = {
//		{ -1.0f,-1.0f,-1.0f},
//		{ 1.0f,-1.0f,-1.0f},
//		{ -1.0f,1.0f,-1.0f},
//		{ 1.0f,1.0f,-1.0f},
//		{ -1.0f,-1.0f,1.0f},
//		{ 1.0f,-1.0f,1.0f},
//		{ -1.0f,1.0f,1.0f},
//		{ 1.0f,1.0f,1.0f},
//	};
//
//	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
//	D3D11_BUFFER_DESC bd = {};
//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.CPUAccessFlags = 0;
//	bd.MiscFlags = 0;
//	bd.ByteWidth = sizeof(vertices);
//	bd.StructureByteStride = sizeof(Vertex);
//	D3D11_SUBRESOURCE_DATA sd = {};
//	sd.pSysMem = vertices;
//	GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
//	const UINT stride = sizeof(Vertex);
//	const UINT offset = 0u;
//	pContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);
//	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	// CREATE indices buffer
//	const unsigned short indices[] = {
//		0,2,1, 2,3,1,
//		1,3,5, 3,7,5,
//		2,6,3, 3,6,7,
//		4,5,7, 4,7,6,
//		0,4,2, 2,4,6,
//		0,1,4, 1,5,4
//	};
//	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
//	D3D11_BUFFER_DESC ibd = {};
//	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	ibd.Usage = D3D11_USAGE_DEFAULT;
//	ibd.CPUAccessFlags = 0;
//	ibd.MiscFlags = 0;
//	ibd.ByteWidth = sizeof(indices);
//	ibd.StructureByteStride = sizeof(unsigned short);
//	D3D11_SUBRESOURCE_DATA isd = {};
//	isd.pSysMem = indices;
//	GFX_THROW_INFO(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));
//
//	// bind index buffer
//	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
//
//	// create constant buffer for transformation matrix
//	struct ConstantBuffer
//	{
//		dx::XMMATRIX transform;
//	};
//
//	const ConstantBuffer cb =
//	{
//		dx::XMMatrixTranspose(dx::XMMatrixRotationX(angle) *
//		dx::XMMatrixRotationZ(angle) *
//		dx::XMMatrixTranslation(x, 0.f, z + 4.f) *
//		dx::XMMatrixPerspectiveLH(1.0f, 3.f/4.f, .5f, 10.f))
//	};
//	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
//	D3D11_BUFFER_DESC cbd = {};
//	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	cbd.Usage = D3D11_USAGE_DYNAMIC;
//	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	cbd.MiscFlags = 0;
//	cbd.ByteWidth = sizeof(cb);
//	cbd.StructureByteStride = 0u;
//	D3D11_SUBRESOURCE_DATA csd = {};
//	csd.pSysMem = &cb;
//	GFX_THROW_INFO(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));
//
//	// bind constant buffer
//	pContext->VSSetConstantBuffers(0, 1, pConstantBuffer.GetAddressOf());
//
//
//	struct ConstantBuffer2
//	{
//		struct 
//		{
//			float r;
//			float g;
//			float b;
//			float a;
//		}face_Color[6];
//	};
//
//	const ConstantBuffer2 cb2 =
//	{
//		{
//			{1.0f,0.0f,1.0f},
//			{1.0f,0.0f,0.0f},
//			{0.0f,1.0f,0.0f},
//			{0.0f,0.0f,1.0f},
//			{1.0f,1.0f,0.0f},
//			{0.0f,1.0f,1.0f},
//		}
//	};
//	wrl::ComPtr<ID3D11Buffer> pConstantBuffer2;
//	D3D11_BUFFER_DESC cbd2 = {};
//	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	cbd2.Usage = D3D11_USAGE_DEFAULT;
//	cbd2.CPUAccessFlags = 0;
//	cbd2.MiscFlags = 0;
//	cbd2.ByteWidth = sizeof(cb2);
//	cbd2.StructureByteStride = 0;
//	D3D11_SUBRESOURCE_DATA csd2 = {};
//	csd2.pSysMem = &cb2;
//	GFX_THROW_INFO(pDevice->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2));
//
//	// bind to pixel shader
//	pContext->PSSetConstantBuffers(0, 1, pConstantBuffer2.GetAddressOf());
//
//	// create vertex shader
//	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
//	wrl::ComPtr<ID3DBlob> pBlob;
//	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
//	GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
//		NULL, &pVertexShader));
//
//	pContext->VSSetShader(pVertexShader.Get(), 0, 0);
//	
//	// input(Vertex) layout
//	wrl::ComPtr<ID3D11InputLayout> pVertexLayout;
//	const D3D11_INPUT_ELEMENT_DESC ied[] = {
//		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//	};
//	GFX_THROW_INFO(pDevice->CreateInputLayout(
//		ied, (UINT)(std::size(ied)),
//		pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
//		&pVertexLayout));
//	
//	// bind vertex layout
//	pContext->IASetInputLayout(pVertexLayout.Get());
//
//	// create vertex shader
//	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
//	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
//	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
//		NULL, &pPixelShader));
//
//	pContext->PSSetShader(pPixelShader.Get(), 0, 0);
//	//pContext->RSSetState()
//		
//	// config viewport
//	D3D11_VIEWPORT vp;
//	vp.Width = 800;
//	vp.Height = 600;
//	vp.MinDepth = 0;
//	vp.MaxDepth = 1;
//	vp.TopLeftX = 0;
//	vp.TopLeftY = 0;
//	pContext->RSSetViewports(1, &vp);
//
//	//GFX_THROW_INFO_ONLY(pContext->Draw((UINT)std::size(vertices), 0));
//	GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(indices), 0, 0));
//}

Graphics::HrException::HrException(int line, const char * file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	// join all info messages with newlines into single string
	for (const auto& m: infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}

	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char * Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String]" << GetErrorString() << std::endl
		<< "[Description]" << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();

	return whatBuffer.c_str();
}

const char * Graphics::HrException::GetType() const noexcept
{
	return "Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char * Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Graphics Exeption [Device Removed](DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char * file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m: infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}

	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char * Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char * Graphics::InfoException::GetType() const noexcept
{
	return "Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
