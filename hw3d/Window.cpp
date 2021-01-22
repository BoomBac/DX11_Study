#include "Window.h"

#include <corecrt_wstdio.h>


LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if (uMsg == WM_NCCREATE)
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window instance
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// forward message to window instance handler
		return pWnd->MessageHandle(hwnd, uMsg, wParam, lParam);
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// retrieve ptr to window instance
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window instance handler
	return pWnd->MessageHandle(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::MessageHandle(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		return 0;
	case WM_KEYDOWN:
		GameInput.OnKeyCaptured(static_cast<unsigned char>(wParam),KeyState::Press);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept  : hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WindowProc;	//回调函数	在窗口进行操作产生反馈
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = nullptr;
	wc.hInstance = WindowClass::GetInstance();
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = (LPCTSTR)GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
}
Window::WindowClass::~WindowClass()
{
	UnregisterClass((LPCWSTR)wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

Window::Window(int width, int height, const LPCWSTR& WindowName) : width(width),height(height)
{
	hWnd = CreateWindow(
		(LPCWSTR)WindowClass::GetName(),WindowName,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);
	if (hWnd == nullptr)
	{
		MessageBox(NULL, TEXT("HWND is nullptr!"), TEXT("Error"), MB_OK);
	}
	RECT rcWindow;
	RECT rcClient;
	GetWindowRect(hWnd, &rcWindow);
	GetClientRect(hWnd, &rcClient);
	auto borderWidth = (rcWindow.right - rcWindow.left)
		- (rcClient.right - rcClient.left);
	auto borderHeight = (rcWindow.bottom - rcWindow.top)
		- (rcClient.bottom - rcClient.top);
	SetWindowPos(hWnd, 0, 0, 0, borderWidth + width, borderHeight + height, SWP_NOMOVE | SWP_NOZORDER);
	GetClientRect(hWnd, &rcClient);

	ShowWindow(hWnd, SW_SHOW);
	//UpdateWindow(hWnd);
}
// 设置窗口标题
void Window::SetText(LPCWSTR NewText)
{
	SetWindowText(hWnd, NewText);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}



