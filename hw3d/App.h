#pragma once
#include "Window.h"
class App
{
public:
	App(int width, int height, LPCWSTR Text);
	~App();
	// ������Ϣ����
	void HandleMessage();
	Window AppWindow;
private:

	// 
	MSG msg = { 0 };
};

