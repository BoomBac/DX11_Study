#include "Graphics.h"
#pragma comment(lib,"d3d11.lib") 

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;


Graphics::Graphics(HWND hWnd)
{
	//����������
	DXGI_SWAP_CHAIN_DESC sd = {};
	//�����������ԡ�������Ҫ��ע�������У���ȡ��߶Ⱥ����ظ�ʽ
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//���ز�����������������
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	//��������Ⱦ����̨������
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//�������еĺ�̨����������������һ��ֻ��һ����̨��������ʵ��˫���档
	sd.BufferCount = 1;
	//��Ҫ��Ⱦ���Ĵ��ڵľ����
	sd.OutputWindow = hWnd;
	//�Ƿ��Դ���ģʽ����
	sd.Windowed = TRUE;
	//��Ϊ DXGI_SWAP_EFFECT_DISCARD�����Կ���������ѡ�����Ч����ʾģʽ
	//sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = 0;

	D3D11CreateDeviceAndSwapChain(nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, pSwap.GetAddressOf(), pDevice.GetAddressOf(), nullptr, pContext.GetAddressOf());

	//��ȡ��̨��������������Ⱦ��ͼ
	ComPtr<ID3D11Resource> pBackBuffer;
	pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));
	pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget);
}


void Graphics::EndFrame()
{
	// ��һͬ�������ˢ����/ʵ��֡��
	ClearBuffer();
	pSwap->Present(1u, 0u);
}
