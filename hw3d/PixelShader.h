#pragma once
#include "Bindable.h"
class PixelShader : public Bindable
{
public:
	//��һΪ��������ɫ������·�� .cso��׺
	PixelShader(LPCWSTR shaderFile, Graphics& gfx);
	void Bind(Graphics& gfx) override;
	ID3DBlob* GetBlob();
private:
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};

