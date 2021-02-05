#include "InputLayout.h"

InputLayout::InputLayout(Graphics& gfx, VertexShader& vs)
{
	vertexlayout->AddInputLayout(LayoutDes::POSITION);
	vertexlayout->AddInputLayout(LayoutDes::NORMAL);
	vertexlayout->AddInputLayout(LayoutDes::TEXCOORD);
	GetDevice(gfx)->CreateInputLayout(vertexlayout->GetD3DLayout().data(), vertexlayout->GetArraySize(), vs.GetBlob()->GetBufferPointer(), vs.GetBlob()->GetBufferSize(), pInputLayout.GetAddressOf());
}

void InputLayout::Bind(Graphics& gfx)
{
	GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
}
