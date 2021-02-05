#pragma once
#include "Bindable.h"
#include <vector>
class IndoxBuffer : Bindable
{
public:
	IndoxBuffer(Graphics& gfx);
	void Bind(Graphics& gfx) override;
private:
	std::vector<UINT> indices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndoxBuffer;
};

