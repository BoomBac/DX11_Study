#pragma once
#include "ChiliWin.h"
#include "input.h"

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
	Window(int width/*���ڿ��*/, int height/*���ڸ߶�*/, const LPCWSTR& WindowName/*���ڱ���*/);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetText(LPCWSTR);

private:
	int width;
	int height;
	HWND hWnd;
	input GameInput;
	//��Ϣ������
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK MessageHandle(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

