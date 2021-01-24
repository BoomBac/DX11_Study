#include "App.h"

App::App(int width, int height, LPCWSTR Text) : AppWindow(width,height,Text)
{
	AppWindow.SetText(Text);
}

App::~App()
{

}
// 分发消息
void App::HandleMessage()
{
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
		}
	}
}

