#pragma once
#include <wrl\client.h>
#include <d3dcommon.h>
#include "Bindable.h"
class VertexShader : public Bindable
{

public:
	//��һΪ��������ɫ������·�� .cso��׺
	VertexShader(LPCWSTR shaderFile, Graphics& gfx);
	void Bind(Graphics& gfx) override;
	ID3DBlob* GetBlob();
private:
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
};



//// ���봴��������ɫ��
//ID3DBlob* pVSBlob = nullptr;
//hr = D3DReadFileToBlob(L"VertexShader.cso", &pVSBlob);
//if (FAILED(hr))
//{
//	MessageBox(NULL,
//		L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
//	return hr;
//}
//hr = pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pVertexShade);