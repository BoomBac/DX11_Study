#include "App.h"

App::App(int width, int height, LPCWSTR Text) : AppWindow(width,height,Text)
{
	AppWindow.SetText(NULL);
}

App::~App()
{

}
// �ַ���Ϣ
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

