#pragma once
#include "Bindable.h"
class PixelShader : public Bindable
{
public:
	//参一为编译后的着色器对象路径 .cso后缀
	PixelShader(LPCWSTR shaderFile, Graphics& gfx);
	void Bind(Graphics& gfx) override;
	ID3DBlob* GetBlob();
private:
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};

