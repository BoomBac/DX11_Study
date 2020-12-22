#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "CusTimer.h"
#include <DirectXColors.h>

#pragma comment(lib,"d3d11.lib") 
#pragma comment(lib,"dxgi.lib") 
#pragma comment(lib,"D3DCompiler.lib")

using namespace DirectX;
using namespace std;

struct SimpleVertex
{
	//XMFLOAT3 pos;
	float X;
	float Y;
	float Z;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};


struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};


ID3D11DepthStencilView* mDepthStencilView;
ID3D11Texture2D* mDepthStencilBuffer = nullptr;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pDeviceContext = nullptr;
IDXGISwapChain* mSwapChain = nullptr;
ID3D11RenderTargetView* mRenderTargetView = nullptr;
ID3D11VertexShader* pVertexShade = nullptr;
ID3D11PixelShader* pPixelShade = nullptr;
ID3D11InputLayout* pVertexLayout = nullptr;
ID3D11Buffer* pVertexBuffer = nullptr;
ID3D11Buffer* pIndoxBuffer = nullptr;
ID3D11Buffer* pConstantBuffer = nullptr;
float backcolor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
CusTimer timer ;
float angle = 0.0f;
HRESULT hr;

ConstantBuffer transformationM;


//const unsigned short indices[] =
//{
//	0,1,2,
//	0,2,3
//};
const unsigned short indices[] =
{
	//每面朝外，
		4,5,6,	//front
		//6,7,4,
		4,6,7,
		1,0,3,//back
		1,3,2,
		0,1,5,  //top
		5,4,0,
		7,6,2,//bottom
		2,3,7,
		1,2,6,//right
		6,5,1,
		3,0,4//left
		,4,7,3
};

HRESULT DrawTriangle();
void Render();
HRESULT DoFrame(HWND hWnd)
{ 
	WCHAR pwch[64] = { 0 };
	//DrawTriangle(timer.Peek());
	angle = -timer.Peek();

	transformationM.mWorld = XMMatrixRotationY(-angle);;
	ConstantBuffer cb = {};
	cb.mWorld = transformationM.mWorld;
	cb.mView = transformationM.mView;
	cb.mProjection = transformationM.mProjection;

	swprintf(pwch, 64, L"Time is%f", angle);
	SetWindowText(hWnd, pwch);

	D3D11_MAPPED_SUBRESOURCE mapSub;
	hr = pDeviceContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSub);
	if (FAILED(hr))
	{
		MessageBox(NULL,L"MAP failed",L"Error",MB_OK);
		return hr;
	}
	memcpy(mapSub.pData, &cb, sizeof(cb));
	pDeviceContext->Unmap(pConstantBuffer, 0);
	//Render();

	const float r = sin(timer.Peek()) / 2.0f + 0.5f;
	const float g = sin(timer.Peek()) / 3.0f + 0.3f;
	const float b = sin(timer.Peek()) / 4.0f + 0.2f;
	backcolor[0] = r;
	backcolor[1] = g;
	backcolor[2] = b;
	pDeviceContext->ClearRenderTargetView(mRenderTargetView, Colors::MidnightBlue);

	pDeviceContext->VSSetShader(pVertexShade, nullptr, 0);
	pDeviceContext->PSSetShader(pPixelShade, nullptr, 0);
	pDeviceContext->DrawIndexed(sizeof(indices) / sizeof(unsigned short), 0, 0);
	mSwapChain->Present(0, 0);
}

bool initdx11(HWND hWnd)
{
	//创建device和context
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0,
		D3D11_CREATE_DEVICE_DEBUG, 0, 0, D3D11_SDK_VERSION, &pDevice, &featureLevel, &pDeviceContext);
	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0); return false;
	}
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D FeatureLevel 11 unsupported.", 0, 0); return false;
	}
	//创建交换链
	DXGI_SWAP_CHAIN_DESC sd = {};
	//缓冲区的属性。我们主要关注的属性有：宽度、高度和像素格式
	sd.BufferDesc.Width = 800;
	sd.BufferDesc.Height =600;
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

	IDXGIDevice* dxgiDevice = nullptr;
	pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	IDXGIAdapter* dxgiAdapter = nullptr;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	IDXGIFactory* dxgiFactory;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	dxgiFactory->CreateSwapChain(pDevice, &sd, &mSwapChain);
	dxgiFactory->Release();
	dxgiAdapter->Release();
	dxgiDevice->Release();
	ID3D11Texture2D* backBuffer;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	hr = pDevice->CreateRenderTargetView(backBuffer, NULL, &mRenderTargetView);
	pDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width =800;
	vp.Height = 600;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	pDeviceContext->RSSetViewports(1, &vp);

	auto a = DrawTriangle();
	if (FAILED(a))
	{
		MessageBox(NULL,
			L"Failed.", L"Error", MB_OK);
	}
	return true;
}

void Render()
{
	if (pDevice)
	{
		const float r = sin(timer.Peek()) / 2.0f + 0.5f;
		const float g = sin(timer.Peek()) / 3.0f + 0.3f;
		const float b = sin(timer.Peek()) / 4.0f + 0.2f;
		backcolor[0] = r;
		backcolor[1] = g;
		backcolor[2] = b;
		pDeviceContext->ClearRenderTargetView(mRenderTargetView, backcolor);
		//DrawTriangle(timer.Peek());

		pDeviceContext->VSSetShader(pVertexShade, nullptr, 0);
		pDeviceContext->PSSetShader(pPixelShade, nullptr, 0);
		pDeviceContext->DrawIndexed(sizeof(indices)/sizeof(unsigned short), 0, 0);
		mSwapChain->Present(0, 0);
		//MessageBox(NULL,
		//	L"execute!", L"Error", MB_OK);
	}

}

void ClearDevice()
{
	if (pDeviceContext)
	{
		pDeviceContext->Release();
	}
	if (pDevice)
	{
		pDevice->Release();
	}
	if (mSwapChain)
	{
		mSwapChain->Release();
	}
	if (mRenderTargetView)
	{
		mRenderTargetView->Release();
	}
}

HRESULT DrawTriangle()
{
	// 编译创建顶点着色器
	ID3DBlob* pVSBlob = nullptr;
	hr = D3DReadFileToBlob(L"VertexShader.cso", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}
	hr = pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pVertexShade);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	//定义输入布局描述
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	UINT numElements = ARRAYSIZE(layout);

	//创建输入布局
	hr = pDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
	{
		return hr;
	}
	//设置输入布局
	pDeviceContext->IASetInputLayout(pVertexLayout);

	// 编译创建像素着色器
	ID3DBlob* pPSBlob = nullptr;
	hr = D3DReadFileToBlob(L"PixelShader.cso", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}
	hr = pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pPixelShade);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;
	// 创建顶点缓冲
	SimpleVertex vertices[] =
	{
		{-0.5f,0.5f, 1.0f,255,0,0},
		{0.5f, 0.5f, 1.0f,0,255,0},
		{0.5f, -0.5f, 1.0f,0,0,255},
		{-0.5f, -0.5f, 1.0f,255,255,255},
		{-0.5f,0.5f, -1.0f,255,0,0},
		{0.5f, 0.5f, -1.0f,0,255,0},
		{0.5f, -0.5f, -1.0f,0,0,255},
		{-0.5f, -0.5f, -1.0f,255,255,255}
	};

	D3D11_BUFFER_DESC bd = {};
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = pDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer);
	if (FAILED(hr))
		return hr;
	// 创建索引缓冲

	D3D11_BUFFER_DESC Ibd = {};
	Ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Ibd.Usage = D3D11_USAGE_DEFAULT;
	Ibd.CPUAccessFlags = 0;
	Ibd.MiscFlags = 0;
	Ibd.ByteWidth = sizeof(indices);
	Ibd.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	hr = pDevice->CreateBuffer(&Ibd, &isd, &pIndoxBuffer);
	pDeviceContext->IASetIndexBuffer(pIndoxBuffer,DXGI_FORMAT_R16_UINT,0);
	// 设置顶点缓冲
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	//创建常量缓冲区

	D3D11_BUFFER_DESC cbd = {};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.StructureByteStride = 0;

	//D3D11_SUBRESOURCE_DATA csd = {};
	//csd.pSysMem = &cb;
	hr = pDevice->CreateBuffer(&cbd, nullptr, &pConstantBuffer);

	pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	// 设置图元拓扑
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//初始化矩阵
	transformationM.mWorld = XMMatrixIdentity();
	XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	transformationM.mView = XMMatrixLookAtLH(Eye, At, Up);
	transformationM.mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 4.0f / 3.0f, 0.01f, 100.f);
	return hr;
}



//回调函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{	
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	case WM_KEYDOWN:
	{
	}
		break;
	case WM_CHAR:		//字符敏感性，当输入文字时使用
	{
		TCHAR buf[1024];
		wsprintf(buf, TEXT("Key is %c"), wParam);
		MessageBox(hwnd, buf, TEXT("key_down"), MB_OK);
	}
		break;
	case WM_LBUTTONDOWN:
	{
		TCHAR buf[1024];
		int xPos = LOWORD(lParam);	// 
		int yPos = HIWORD(lParam);	// 
		wsprintf(buf, TEXT("x = %d,y = %d"), xPos, yPos);
		MessageBox(hwnd, buf, TEXT("mouse down"), MB_OK);
	}
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPreInstance,LPSTR lpCmdLine,int nCmdShow)
{

	const auto pClassName = TEXT("hw3d");
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WindowProc;	//回调函数	在窗口进行操作产生反馈
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = nullptr;
	wc.hInstance = hInstance;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = pClassName;
	wc.hIconSm = nullptr;

	RegisterClassEx(&wc);
	HWND hWnd = CreateWindowEx(0, (LPWSTR)pClassName,TEXT("MyWindow"),
		WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU,200,200,800,600,nullptr,nullptr,hInstance,nullptr);

	ShowWindow(hWnd,SW_SHOW);
	MSG msg = {0};
	initdx11(hWnd);
	while (WM_QUIT!= msg.message)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			DoFrame(hWnd);
			//Render();
		}
	}
	ClearDevice();
	return 0;
}

