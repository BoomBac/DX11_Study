//#include <d3d11.h>
//#pragma comment(lib,"d3d11.lib") 
//#pragma comment(lib,"dxgi.lib") 
//
//
//bool initdx11(HWND hWnd)
//{
//	//创建device和context
//	D3D_FEATURE_LEVEL featureLevel;
//	ID3D11Device* pDevice;
//	ID3D11DeviceContext* pDeviceContext;
//	HRESULT hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, 
//		D3D11_CREATE_DEVICE_DEBUG, 0, 0, D3D11_SDK_VERSION, &pDevice, &featureLevel, &pDeviceContext);
//	if (FAILED(hr)) 
//	{
//		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0); return false;
//	}
//	if (featureLevel != D3D_FEATURE_LEVEL_11_0) 
//	{
//		MessageBox(0, L"Direct3D FeatureLevel 11 unsupported.", 0, 0); return false;
//	}
//	//创建交换链
//	DXGI_SWAP_CHAIN_DESC sd = {};
//	//缓冲区的属性。我们主要关注的属性有：宽度、高度和像素格式
//	sd.BufferDesc.Width = 0;
//	sd.BufferDesc.Height = 0;
//	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
//	sd.BufferDesc.RefreshRate.Numerator = 0;
//	sd.BufferDesc.RefreshRate.Denominator = 0;
//	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
//	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
//
//	//多重采样数量和质量级别
//	sd.SampleDesc.Count = 1;
//	sd.SampleDesc.Quality = 0;
//
//	//将场景渲染到后台缓冲区
//	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//	//交换链中的后台缓冲区数量；我们一般只用一个后台缓冲区来实现双缓存。
//	sd.BufferCount = 1;
//	//将要渲染到的窗口的句柄。
//	sd.OutputWindow = hWnd;
//	//是否以窗口模式运行
//	sd.Windowed = TRUE;
//	//设为 DXGI_SWAP_EFFECT_DISCARD，让显卡驱动程序选择最高效的显示模式
//	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
//	sd.Flags = 0;
//
//	IDXGIFactory* dxgiFactory;
//	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&dxgiFactory));
//	IDXGISwapChain* mSwapChain;
//	dxgiFactory->CreateSwapChain(pDevice, &sd, &mSwapChain);
//
//	ID3D11RenderTargetView* mRenderTargetView;
//	ID3D11Texture2D* backBuffer;
//	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
//	pDevice->CreateRenderTargetView(backBuffer,NULL,&mRenderTargetView);
//
//	D3D11_TEXTURE2D_DESC depthStencilDesc;
//	depthStencilDesc.Width = 800;
//	depthStencilDesc.Height = 600;
//	depthStencilDesc.MipLevels = 1;
//	depthStencilDesc.ArraySize = 1;
//	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	depthStencilDesc.CPUAccessFlags = 0;
//	depthStencilDesc.SampleDesc.Count = 1;
//	depthStencilDesc.SampleDesc.Quality = 0;
//	depthStencilDesc.MiscFlags = 0;
//	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
//	ID3D11Texture2D* mDepthStencilBuffer;
//	ID3D11DepthStencilView* mDepthStencilView;
//	pDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer);
//	pDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView);
//
//	pDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
//
//	D3D11_VIEWPORT vp;
//	vp.TopLeftX = 0;
//	vp.TopLeftY = 0;
//	vp.Width = 800;
//	vp.Height = 600;
//	vp.MinDepth = 0.0f;
//	vp.MaxDepth = 1.0f;
//
//	pDeviceContext->RSSetViewports(1, &vp);
//
//	return true;
//}