#pragma once
#include "Bindable.h"
#include "Vertex.h"
#include "VertexShader.h"
#include <memory>


class InputLayout : public Bindable
{
public:
	InputLayout(Graphics& gfx,VertexShader& vs);
	void Bind(Graphics& gfx) override;

protected:
//	Microsoft::WRL::ComPtr<VertexLayout>vertexlayout;
	std::shared_ptr<VertexLayout> vertexlayout;
	//std::unique_ptr<VertexLayout> vertexlayout;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
};

