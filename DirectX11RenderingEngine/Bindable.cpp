#include "Bindable.h"

ID3D11DeviceContext* Bindable::GetContext(Graphics &gfx) noexcept
{
	return gfx.pContext.Get();
}

ID3D11Device* Bindable::GetDevice(Graphics &gfx) noexcept
{
	return gfx.pDevice.Get();
}

DxgiInfoManager& Bindable::GetInfoManager(Graphics &gfx) noexcept(!IS_DEBUG)
{
#ifndef NDEBUG
	return gfx.infoManager;
#else
	throw std::logic_error("You FXXX up! (tried to acess gfx.informanger in release config");
#endif
}