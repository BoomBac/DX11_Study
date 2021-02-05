#include "VertexShader.h"
#include <d3dcompiler.h>

VertexShader::VertexShader(LPCWSTR shaderFile,Graphics& gfx)
{
	//LPCWSTR FileName = shaderFile + '.cso';
	D3DReadFileToBlob(shaderFile, pBlob.GetAddressOf());
	GetDevice(gfx)->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pVertexShader.GetAddressOf());
}

void VertexShader::Bind(Graphics& gfx)
{
	GetContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0);
}

ID3DBlob* VertexShader::GetBlob()
{
	return pBlob.Get();
}
