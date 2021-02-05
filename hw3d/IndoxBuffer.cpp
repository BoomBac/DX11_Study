#include "IndoxBuffer.h"

IndoxBuffer::IndoxBuffer(Graphics& gfx)
{
	// 创建索引缓冲

	D3D11_BUFFER_DESC Ibd = {};
	Ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Ibd.Usage = D3D11_USAGE_DEFAULT;
	Ibd.CPUAccessFlags = 0;
	Ibd.MiscFlags = 0;
	Ibd.ByteWidth = UINT(sizeof(UINT) * indices.size());
	Ibd.StructureByteStride = sizeof(UINT);

	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();
	GetDevice(gfx)->CreateBuffer(&Ibd, &isd, pIndoxBuffer.GetAddressOf());
}

void IndoxBuffer::Bind(Graphics& gfx)
{
	GetContext(gfx)->IASetIndexBuffer(pIndoxBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}
//// 创建索引缓冲
//
//D3D11_BUFFER_DESC Ibd = {};
//Ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//Ibd.Usage = D3D11_USAGE_DEFAULT;
//Ibd.CPUAccessFlags = 0;
//Ibd.MiscFlags = 0;
//Ibd.ByteWidth = sizeof(indices);
//Ibd.StructureByteStride = sizeof(unsigned short);
//
//D3D11_SUBRESOURCE_DATA isd = {};
//isd.pSysMem = indices;
//hr = pDevice->CreateBuffer(&Ibd, &isd, &pIndoxBuffer);