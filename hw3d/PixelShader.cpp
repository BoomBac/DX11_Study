#include "PixelShader.h"
#include <d3dcompiler.h>

PixelShader::PixelShader(LPCWSTR shaderFile, Graphics& gfx)
{
	D3DReadFileToBlob(shaderFile, pBlob.GetAddressOf());
	GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pPixelShader.GetAddressOf());
}

void PixelShader::Bind(Graphics& gfx)
{
	GetContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0);
}

ID3DBlob* PixelShader::GetBlob()
{
	return pBlob.Get();
}
