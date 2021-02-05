#pragma once
#include "Bindable.h"
#include "Vertex.h"

class VertexBuffer : public Bindable
{
public:
	VertexBuffer(Graphics& gfx);
	void Bind(Graphics& gfx) override;
private:
	std::vector<SimpleVertex> vertices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};
VertexBuffer::VertexBuffer(Graphics& gfx)
{
	D3D11_BUFFER_DESC vbd = {};
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = UINT(sizeof(SimpleVertex) * vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices.data();
	GetDevice(gfx)->CreateBuffer(&vbd, &InitData, pVertexBuffer.GetAddressOf());
}
void VertexBuffer::Bind(Graphics& gfx)
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	GetContext(gfx)->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);
}
