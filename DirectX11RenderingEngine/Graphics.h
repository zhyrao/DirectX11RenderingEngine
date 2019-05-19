#pragma once

#include "Win.h"
#include "ExceptionClass.h"
#include <d3d11.h>
#include <vector>
#include "DxgiInfoManager.h"


class Graphics
{
public:
	class Exception : public ExceptionClass
	{
		using ExceptionClass::ExceptionClass;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};

	class DeviceRemovedException : public HrException
	{
		using HrException :: HrException;
	public:
		const char* GetType() const noexcept override;
	private: 
		std::string reason;
	};
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();

	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept;
private:
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	IDXGISwapChain* pSwapChain = NULL;
	ID3D11Device* pDevice = NULL;
	ID3D11DeviceContext* pContext = NULL;
	ID3D11RenderTargetView* pTarget = NULL;
};

