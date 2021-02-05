#include "Graphics.h"
#pragma comment(lib,"d3d11.lib") 

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;


Graphics::Graphics(HWND hWnd)
{
	//创建交换链
	DXGI_SWAP_CHAIN_DESC sd = {};
	//缓冲区的属性。我们主要关注的属性有：宽度、高度和像素格式
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//多重采样数量和质量级别
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	//将场景渲染到后台缓冲区
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//交换链中的后台缓冲区数量；我们一般只用一个后台缓冲区来实现双缓存。
	sd.BufferCount = 1;
	//将要渲染到的窗口的句柄。
	sd.OutputWindow = hWnd;
	//是否以窗口模式运行
	sd.Windowed = TRUE;
	//设为 DXGI_SWAP_EFFECT_DISCARD，让显卡驱动程序选择最高效的显示模式
	//sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = 0;

	D3D11CreateDeviceAndSwapChain(nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, pSwap.GetAddressOf(), pDevice.GetAddressOf(), nullptr, pContext.GetAddressOf());

	//获取后台缓冲区，创建渲染视图
	ComPtr<ID3D11Resource> pBackBuffer;
	pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));
	pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget);
}


void Graphics::EndFrame()
{
	// 参一同步间隔：刷新率/实际帧数
	ClearBuffer();
	pSwap->Present(1u, 0u);
}
