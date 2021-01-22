#pragma once
#include "Window.h"
class App
{
public:
	App(int width, int height, LPCWSTR Text);
	~App();
	// 窗口消息处理
	void HandleMessage();
	Window AppWindow;
private:

	// 
	MSG msg = { 0 };
};

