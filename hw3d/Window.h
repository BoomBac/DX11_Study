#pragma once
#include "ChiliWin.h"
#include "input.h"
#include "Graphics.h"
#include <memory>

//class input;
class App;
class Window
{
	friend App;
	class WindowClass
	{
	public:
		static const char* GetName()noexcept;
		static HINSTANCE GetInstance()noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "Direct3D Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width/*窗口宽度*/, int height/*窗口高度*/, const LPCWSTR& WindowName/*窗口标题*/);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Graphics& Gfx();
	void SetText(LPCWSTR);
	HWND GetHWND();
private:
	int width;
	int height;
	HWND hWnd;
	input GameInput;
	std::unique_ptr<Graphics> pGfx;
	//消息处理函数
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK MessageHandle(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

