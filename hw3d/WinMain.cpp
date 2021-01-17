#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "CusTimer.h"
#include "WICTextureLoader11.h"
#include "GeometryGenerator.h"

#pragma comment(lib,"d3d11.lib") 
#pragma comment(lib,"dxgi.lib") 
#pragma comment(lib,"D3DCompiler.lib")
#pragma comment( lib, "dxguid.lib")

using namespace DirectX;
using namespace std;

struct PointLight
{
	XMFLOAT4 Color;
	XMFLOAT4 Position;
};



struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX WorldInvTranspose;
	XMMATRIX mView;
	XMMATRIX mProjection;
	PointLight plight;
	XMMATRIX scaling;
	XMMATRIX Reflect;
	bool isReflect;
};

// 平面点个数
int planeVertexCount = 10;

//相关尺寸
float Size_x = 800;
float Size_y = 600;

ID3D11DepthStencilView* pDepthStencilView;
ID3D11Texture2D* pDepthStencilBuffer = nullptr;
ID3D11DepthStencilState* pMarkMirrorDSS = nullptr;
ID3D11DepthStencilState* pDrawReflectionDSS = nullptr;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pDeviceContext = nullptr;
IDXGISwapChain* mSwapChain = nullptr;
ID3D11RenderTargetView* mRenderTargetView = nullptr;
ID3D11VertexShader* pVertexShade = nullptr;
ID3D11VertexShader* pVertexShade1 = nullptr;
ID3D11PixelShader* pPixelShade = nullptr;
ID3D11PixelShader* pPixelShade1 = nullptr;
ID3D11InputLayout* pVertexLayout = nullptr;
ID3D11Buffer* pVertexBuffer = nullptr;
ID3D11Buffer* pIndoxBuffer = nullptr;
ID3D11Buffer* pConstantBuffer = nullptr;
ID3D11SamplerState* pSamState = nullptr;
ID3D11ShaderResourceView* pShaderRs;
ID3D11ShaderResourceView* pShaderRs1;
ID3D11BlendState* pBlendState;
UINT stride = sizeof(SimpleVertex);
UINT offset = 0;

//摄像机位移相关
struct CameraTransform
{
	struct 
	{
		float x;
		float y;
		float z;
		float w;
	} axis;
	struct 
	{
		float dx;
		float dy;
		float dz;
	} delta;
};
CameraTransform ins_CameraT = { 1.f, 1.f, 1.f, 0.0f,
1.f,1.f,1.f};
XMVECTOR Eye = XMVectorSet(ins_CameraT.axis.x, ins_CameraT.axis.y, ins_CameraT.axis.z, ins_CameraT.axis.w);
//摄像机射线相关
struct CameraView
{
	float x;
	float y;
	float z;
};
CameraView ins_CameraV = { 0.f,0.f,1.f };

//鼠标移动
struct MouseMove
{
	int xPos;
	int yPos;
	float xDelta;
	float yDelta;
};
MouseMove ins_Mousemove = { 0,0,0,0 };

float backcolor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
CusTimer timer ;
float angle = 0.0f;
HRESULT hr;

// 立方体世界矩阵Y偏移
auto cubeMove = 0.f;		//记录当前偏移量
float CubeTransformation= 0.f;	//cube目标偏移量
float LightTransformation = 50.f;	//light目标偏移量
const float LerpSpeed = 0.995f;

const auto c = GET_INDOX_AMOUNT(40, 40);
unsigned short indices[2*c] = { 0 };

const auto i = GET_POINTAMOUNT(40, 40);
SimpleVertex vertices[2*i] = { 0 };

ConstantBuffer transformationM;

bool SaveDepthStencilBuffer()
{
	D3D11_TEXTURE2D_DESC dsDesc, destTexDesc;
	ID3D11Texture2D* destTex;
	HRESULT result;

	if (pDepthStencilBuffer)
	{
		pDepthStencilBuffer->GetDesc(&dsDesc);
		// 使目的和源的描述一致
		memcpy(&destTexDesc, &dsDesc, sizeof(destTexDesc));
		destTexDesc.Usage = D3D11_USAGE_STAGING;
		destTexDesc.BindFlags = 0;
		destTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		result = pDevice->CreateTexture2D(&destTexDesc, 0, &destTex);
		if (FAILED(result))
		{
			return false;
		}

		//depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		pDeviceContext->CopyResource(destTex, pDepthStencilBuffer);

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		result = pDeviceContext->Map(destTex, 0, D3D11_MAP_READ, 0, &mappedResource);
		if (FAILED(result))
		{
			return false;
		}

		FILE* fp = fopen("depth-stencil.csv", "w");

		const UINT WIDTH = destTexDesc.Width;
		const UINT HEIGHT = destTexDesc.Height;
		//映射为32位的dword
		DWORD* pTexels = (DWORD*)mappedResource.pData;
		for (UINT row = 0; row < HEIGHT; row++)
		{
			UINT rowStart = row * mappedResource.RowPitch / sizeof(pTexels[0]);
			for (UINT col = 0; col < WIDTH; col++)
			{
				fprintf(fp, "%08x,", pTexels[rowStart + col]);
			}
			fprintf(fp, "\n");
		}

		fclose(fp);
		pDeviceContext->Unmap(destTex, 0);
	}
	return true;

}


void lerp(float &target, float &current, float alpha)
{
	current = alpha * current + (1 - alpha) * target;
}
bool BuildBlendState()
{
	D3D11_BLEND_DESC blendStateDescription;
	// 初始化blend描述符 
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// 创建一个alpha blend状态. 
	blendStateDescription.AlphaToCoverageEnable = FALSE;
	blendStateDescription.IndependentBlendEnable = FALSE;
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;		//alpha(source)
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	//1-alpha(source)
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(pDevice->CreateBlendState(&blendStateDescription, &pBlendState)))
	{
		MessageBox(NULL, TEXT("Create 'Transparent' blend state failed!"), TEXT("Error"), MB_OK);
		return false;
	}

	return true;
}
bool BuildDepthState()
{
	D3D11_DEPTH_STENCIL_DESC mirrorDesc; 
	mirrorDesc.DepthEnable= true;
	mirrorDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; 
	mirrorDesc.DepthFunc = D3D11_COMPARISON_LESS;
	mirrorDesc.StencilEnable= true;
	mirrorDesc.StencilReadMask = 0xff; 
	mirrorDesc.StencilWriteMask = 0xff;
	mirrorDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; 
	mirrorDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mirrorDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		 
	// We are not rendering backfacing polygons, so these settings do not matter. mirrorDesc.BackFace.StencilFailOp
	mirrorDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR; 
	mirrorDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mirrorDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS; 
	pDevice->CreateDepthStencilState(&mirrorDesc, &pMarkMirrorDSS);

	// // DrawReflectionDSS //
	D3D11_DEPTH_STENCIL_DESC drawReflectionDesc; 
	drawReflectionDesc.DepthEnable= true;
	drawReflectionDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; 
	drawReflectionDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	drawReflectionDesc.StencilEnable = true;
	drawReflectionDesc.StencilReadMask = 0xff; 
	drawReflectionDesc.StencilWriteMask = 0xff;
	drawReflectionDesc.FrontFace.StencilFailOp=D3D11_STENCIL_OP_KEEP; 
	drawReflectionDesc.FrontFace.StencilDepthFailOp= D3D11_STENCIL_OP_KEEP; 
	drawReflectionDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP; 
	drawReflectionDesc.FrontFace.StencilFunc= D3D11_COMPARISON_EQUAL;
	drawReflectionDesc.BackFace.StencilFailOp =D3D11_STENCIL_OP_KEEP; 
	drawReflectionDesc.BackFace.StencilDepthFailOp=D3D11_STENCIL_OP_KEEP; 
	drawReflectionDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP; 
	drawReflectionDesc.BackFace.StencilFunc= D3D11_COMPARISON_EQUAL;
	pDevice->CreateDepthStencilState(&drawReflectionDesc, &pDrawReflectionDSS);
	return true;
}

HRESULT DrawTriangle();
void Render();
HRESULT DoFrame(HWND hWnd)
{
	pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	pDeviceContext->ClearRenderTargetView(mRenderTargetView, backcolor);
	angle = timer.Peek();
	//transformationM.mWorld = XMMatrixRotationY(angle);
	transformationM.mWorld = XMMatrixIdentity();

	lerp(ins_CameraT.delta.dx, ins_CameraT.axis.x, LerpSpeed);
	lerp(ins_CameraT.delta.dy, ins_CameraT.axis.y, LerpSpeed);
	lerp(ins_CameraT.delta.dz, ins_CameraT.axis.z, LerpSpeed);
	//auto c = (ins_Mousemove.xDelta) / Size_x + ins_CameraV.x;

	lerp(ins_Mousemove.xDelta, ins_CameraV.x, LerpSpeed);
	lerp(ins_Mousemove.yDelta, ins_CameraV.y, LerpSpeed);
	WCHAR pwch[64] = { 0 };
	swprintf(pwch, 64, L"angle is%f", c);
	SetWindowText(hWnd, pwch);

	transformationM.mView = XMMatrixLookToLH({ ins_CameraT.axis.x,ins_CameraT.axis.y,ins_CameraT.axis.z,ins_CameraT.axis.w },//eye_position
		{ ins_CameraV.x,ins_CameraV.y,1,0.f }, //at
		{ 0.f,1.f,0.0f,0.f }	//up
	);
	ConstantBuffer cb = {};
	cb.mWorld = transformationM.mWorld;
	cb.mView = transformationM.mView;
	cb.mProjection = transformationM.mProjection;
	cb.plight.Color = transformationM.plight.Color;
	cb.plight.Position = transformationM.plight.Position;
	cb.isReflect = false;
	// row
	cb.scaling = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	lerp(LightTransformation, transformationM.plight.Position.y, LerpSpeed);


	D3D11_MAPPED_SUBRESOURCE mapSub;
	hr = pDeviceContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSub);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"MAP failed", L"Error", MB_OK);
		return hr;
	}
	memcpy(mapSub.pData, &cb, sizeof(cb));
	pDeviceContext->Unmap(pConstantBuffer, 0);
	const float r = sin(timer.Peek()) / 2.0f + 0.5f;
	const float g = sin(timer.Peek()) / 3.0f + 0.3f;
	const float b = sin(timer.Peek()) / 4.0f + 0.2f;
	backcolor[0] = 0.0f;
	backcolor[1] = 0.75f;
	backcolor[2] = 1.0f;
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };



	pDeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);
	pDeviceContext->VSSetShader(pVertexShade, nullptr, 0);
	pDeviceContext->PSSetShader(pPixelShade, nullptr, 0);
	pDeviceContext->PSSetShaderResources(0, 1, &pShaderRs1);
	pDeviceContext->PSSetSamplers(0, 1, &pSamState);

	pDeviceContext->OMSetDepthStencilState(pMarkMirrorDSS, 1);
	pDeviceContext->OMSetBlendState(pBlendState, blendFactor, 0xffffffff);
	pDeviceContext->DrawIndexed(GET_INDOX_AMOUNT(planeVertexCount, planeVertexCount), 3 * GET_INDOX_AMOUNT(planeVertexCount, planeVertexCount), 0);
	pDeviceContext->OMSetDepthStencilState(0, 1);
	pDeviceContext->OMSetBlendState(NULL, blendFactor, 0xffffffff);

	// 墙面
	lerp(CubeTransformation, cubeMove, LerpSpeed);
	transformationM.mWorld =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,cubeMove,0,1
	};
	cb.mWorld = transformationM.mWorld;
	cb.scaling = {
	1,0,0,/*angle * 0.1f*/0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
	};
	hr = pDeviceContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSub);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"MAP failed", L"Error", MB_OK);
		return hr;
	}
	memcpy(mapSub.pData, &cb, sizeof(cb));
	pDeviceContext->Unmap(pConstantBuffer, 0);

	pDeviceContext->VSSetShader(pVertexShade1, nullptr, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	pDeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);
	pDeviceContext->PSSetShaderResources(0, 1, &pShaderRs);
	pDeviceContext->PSSetSamplers(0, 1, &pSamState);

	pDeviceContext->DrawIndexed(GET_INDOX_AMOUNT(planeVertexCount, planeVertexCount), 0, 0);
	pDeviceContext->DrawIndexed(GET_INDOX_AMOUNT(planeVertexCount, planeVertexCount), GET_INDOX_AMOUNT(planeVertexCount, planeVertexCount), 0);
	pDeviceContext->DrawIndexed(GET_INDOX_AMOUNT(planeVertexCount, planeVertexCount), 2 * GET_INDOX_AMOUNT(planeVertexCount, planeVertexCount), 0);

	pDeviceContext->DrawIndexed(36, 4 * GET_INDOX_AMOUNT(planeVertexCount, planeVertexCount), 0);
	pDeviceContext->DrawIndexed(36, 36+36+ 4 * GET_INDOX_AMOUNT(planeVertexCount, planeVertexCount), 0);
	//绘制镜子之后的物体
	auto  RefMat = XMMatrixReflect({ 0.f, 25.f, 0.f });
	cb.Reflect =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0.f,0.f,30.f,1
	};
	cb.isReflect = true;
	hr = pDeviceContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSub);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"MAP failed", L"Error", MB_OK);
		return hr;
	}
	memcpy(mapSub.pData, &cb, sizeof(cb));
	pDeviceContext->Unmap(pConstantBuffer, 0);
	pDeviceContext->OMSetDepthStencilState(pDrawReflectionDSS, 1);
	pDeviceContext->DrawIndexed(36, 4 * GET_INDOX_AMOUNT(planeVertexCount, planeVertexCount)+36, 0);
	cb.Reflect =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0.f,0.f,70.f,1
	};
	hr = pDeviceContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSub);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"MAP failed", L"Error", MB_OK);
		return hr;
	}
	memcpy(mapSub.pData, &cb, sizeof(cb));
	pDeviceContext->Unmap(pConstantBuffer, 0);

	pDeviceContext->DrawIndexed(36, 4 * GET_INDOX_AMOUNT(planeVertexCount, planeVertexCount)+36*3, 0);

	pDeviceContext->OMSetDepthStencilState(NULL, 1);


	//交换前后台缓冲区
	mSwapChain->Present(0, 0);

	return hr;
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
	sd.BufferDesc.Width = Size_x;
	sd.BufferDesc.Height =Size_y;
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
	//创建深度模板缓冲区
	D3D11_TEXTURE2D_DESC dsd;
	dsd.Width = Size_x;
	dsd.Height = Size_y;
	dsd.MipLevels = 1;
	dsd.ArraySize = 1;
	dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsd.SampleDesc.Count = 1;
	dsd.SampleDesc.Quality = 0;
	dsd.Usage = D3D11_USAGE_DEFAULT;
	dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsd.CPUAccessFlags = 0;
	dsd.MiscFlags = 0;
	pDevice->CreateTexture2D(&dsd, 0, &pDepthStencilBuffer);
	pDevice->CreateDepthStencilView(pDepthStencilBuffer, 0, &pDepthStencilView);
	pDeviceContext->OMSetRenderTargets(1, &mRenderTargetView,pDepthStencilView);


	// 创建视口
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width =Size_x;
	vp.Height = Size_y;
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

		pDeviceContext->VSSetShader(pVertexShade, nullptr, 0);
		pDeviceContext->PSSetShader(pPixelShade, nullptr, 0);
		pDeviceContext->DrawIndexed(sizeof(indices)/sizeof(unsigned short), 0, 0);
		mSwapChain->Present(0, 0);
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
	hr = D3DReadFileToBlob(L"VertexShader1.cso", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}
	hr = pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pVertexShade1);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	//定义输入布局描述
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},
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
	hr = D3DReadFileToBlob(L"PixelShader1.cso", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}
	hr = pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pPixelShade1);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;
	// 创建顶点缓冲



	// 3wall
	GeometryGenerator::GeneratePlane(100.f, 100.f, planeVertexCount, planeVertexCount, vertices, indices, { 0.f,1.f,0.f }, {0.f,0.f,0.f});
	GeometryGenerator::GeneratePlane(100.f, 100.f, planeVertexCount, planeVertexCount, vertices, indices, { 0.f,0.f,1.f }, {0.f,0.f,50.f});
	GeometryGenerator::GeneratePlane(100.f, 100.f, planeVertexCount, planeVertexCount, vertices, indices, { 1.f,0.f,0.f }, {-50.f,0.f,0.f});
	//mirror
	GeometryGenerator::GeneratePlane(20.f, 20.f, planeVertexCount, planeVertexCount, vertices, indices, { 0.f,0.f,1.f }, {0.f,10.f,25.f});
	//cube
	GeometryGenerator::GenerateBox(30.f, 5.f, 5.f, vertices, indices, GET_INDOX_AMOUNT(planeVertexCount,planeVertexCount), 0, { 0.f,0.f,10.f });
	//cube_ref
	GeometryGenerator::GenerateBox(30.f, 5.f, 5.f, vertices, indices, GET_INDOX_AMOUNT(planeVertexCount,planeVertexCount), 0, { 0.f,0.f,10.f });
	//cube1
	GeometryGenerator::GenerateBox(10.f, 30.f, 10.f, vertices, indices, GET_INDOX_AMOUNT(planeVertexCount,planeVertexCount), 0, { 0.f,10.f,-10.f });
	//cube1_ref
	GeometryGenerator::GenerateBox(10.f, 30.f, 10.f, vertices, indices, GET_INDOX_AMOUNT(planeVertexCount,planeVertexCount), 0, { 0.f,10.f,-10.f });


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

	pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	//创建常量缓冲区

	D3D11_BUFFER_DESC cbd = {};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.StructureByteStride = 0;

	hr = pDevice->CreateBuffer(&cbd, nullptr, &pConstantBuffer);

	pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	// 设置图元拓扑
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//初始化矩阵
	transformationM.mWorld = XMMatrixIdentity();
	transformationM.WorldInvTranspose = XMMatrixTranspose(transformationM.mWorld);

	XMVECTOR At = XMVectorSet(0.0f, 0.f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	transformationM.mView = XMMatrixLookAtLH(Eye, At, Up);
	transformationM.mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV2, Size_x/Size_y, 0.01f, 1000.f);
	transformationM.plight.Color = { 0.0f,1.0f,0.f,1.f };
	transformationM.plight.Position = { 0.f,50.f,0.f,1.f };
	//混合
	BuildBlendState();
	//深度/模板缓冲状态
	BuildDepthState();

	//纹理贴图
	hr = CreateWICTextureFromFile(pDevice, L"Texture/farbic.jpeg", nullptr, &pShaderRs);

	hr = CreateWICTextureFromFile(pDevice, L"Texture/mirror.jpg", nullptr, &pShaderRs1);
	D3D11_SAMPLER_DESC samD;
	ZeroMemory(&samD, sizeof(samD));
	samD.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samD.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samD.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samD.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samD.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samD.MinLOD = 0;
	samD.MaxLOD = D3D11_FLOAT32_MAX;
	pDevice->CreateSamplerState(&samD, &pSamState);
	//pDeviceContext->OMSetDepthStencilState(pD)

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
		switch (wParam)
		{
		case VK_UP:
			ins_CameraT.delta.dz += 10.f;
			break;		
		case VK_DOWN:
			ins_CameraT.delta.dz -= 10.f;
			break;		
		case VK_LEFT:
			ins_CameraT.delta.dx -= 10.f;
			break;		
		case VK_RIGHT:
			ins_CameraT.delta.dx += 10.f;
			break;
		case VK_ADD:
			ins_CameraT.delta.dy += 10.f;
			break;
		case VK_SUBTRACT:
			ins_CameraT.delta.dy -= 10.f;
			break;
		default:
			break;
		}
	}
		break;
	case WM_CHAR:		//字符敏感性，当输入文字时使用
	{
	}
		break;
	case WM_LBUTTONDOWN:
	{
		LightTransformation += 20.f;
		//CubeTransformation += 10.f;
		
		ins_Mousemove.xPos = LOWORD(lParam);
		ins_Mousemove.yPos = HIWORD(lParam);

	}
		break;
	case WM_RBUTTONDOWN:
	{
		LightTransformation -= 20.f;
		//CubeTransformation -= 10.f;
		SaveDepthStencilBuffer();
	}
		break;
	case WM_LBUTTONUP:
	{
		ins_Mousemove.xDelta =(LOWORD(lParam) - ins_Mousemove.xPos)/Size_x + ins_CameraV.x;
		ins_Mousemove.yDelta = (HIWORD(lParam) - ins_Mousemove.yPos)/Size_y + ins_CameraV.y;
	}
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
		WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU,0,0,Size_x,Size_y,nullptr,nullptr,hInstance,nullptr);
	RECT rcWindow;
	RECT rcClient;
	GetWindowRect(hWnd, &rcWindow);
	GetClientRect(hWnd, &rcClient);
	auto borderWidth = (rcWindow.right - rcWindow.left)
		- (rcClient.right - rcClient.left);
	auto borderHeight = (rcWindow.bottom - rcWindow.top)
		- (rcClient.bottom - rcClient.top);
	SetWindowPos(hWnd, 0, 0, 0, borderWidth + Size_x, borderHeight + Size_y, SWP_NOMOVE | SWP_NOZORDER);
	GetClientRect(hWnd, &rcClient);
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
		}
	}
	ClearDevice();
	return 0;
}

