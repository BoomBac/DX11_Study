#pragma once
#include "ChiliWin.h"
#include <d3d11.h>
#include <wrl.h>

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class Graphics
{
	friend class Bindable;
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;
	//交换前后缓冲区
	void EndFrame();
	void ClearBuffer()
	{
		float color[] = { 1.f,0.f,0.f,0.f };
		pContext->ClearRenderTargetView(pTarget.Get(), color);
	}
private:
	ComPtr<ID3D11Device> pDevice = nullptr;
	ComPtr<IDXGISwapChain> pSwap = nullptr;
	ComPtr<ID3D11DeviceContext> pContext = nullptr;
	ComPtr<ID3D11RenderTargetView> pTarget = nullptr;
};

