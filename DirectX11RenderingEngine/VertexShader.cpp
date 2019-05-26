#include "VertexShader.h"
#include "GraphicsThrowMacros.h"

VertexShader::VertexShader(Graphics & gfx, const std::wstring & path)
{
	INFOMAN(gfx);

	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pBytecodeBlob));
	GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		NULL,
		&pVertexShader
	));
}

void VertexShader::Bind(Graphics & gfx) noexcept
{
	GetContext(gfx)->VSSetShader(pVertexShader.Get(), NULL, 0u);
}

ID3DBlob* VertexShader::GetByteCode() const noexcept
{
	return pBytecodeBlob.Get();
}

