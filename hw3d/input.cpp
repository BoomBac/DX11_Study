#include "input.h"
#include "Tool.h"


using namespace std;
// 按键默认操作函数
void DefaultOp()
{
	OutputDebugString(TEXT("DefaultOp"));
}

input::Event::Event(unsigned char keycode, KeyState keystate, OperateBind Op) : KeyCode(keycode), Keystate(keystate), OperateFunc(Op)
{
	if (Op == nullptr)
		OperateFunc = DefaultOp;
}



void input::AddKeyBind(unsigned char keycode, OperateBind Bop, KeyState keystate)
{

	if (keystate == KeyState::Press)
	{
		KeyPressOp.insert(pair<unsigned char, OperateBind>(keycode, Bop));
	}
	if (keystate == KeyState::Release)
	{
		KeyReleaseOp.insert(pair<unsigned char, OperateBind>(keycode, Bop));
	}
}

void input::OnKeyCaptured(unsigned char keycode, KeyState keystate)
{
	if(keystate==KeyState::Press)
	{
		if (KeyPressOp.size() == 0)
		{
			EventQueue.push(input::Event(keycode, KeyState::Press, nullptr));
		}
		else
		{
			EventQueue.push(input::Event(keycode, KeyState::Press, KeyPressOp.find(keycode)->second));
		}
	}
	if (keystate == KeyState::Release)
	{
		if (KeyPressOp.size() == 0)
		{
			EventQueue.push(input::Event(keycode, KeyState::Release, nullptr));
		}
		else
		{
			EventQueue.push(input::Event(keycode, KeyState::Release, KeyReleaseOp.find(keycode)->second));
		}
	}
	//测试用
	//OutputDebugString(TEXT("%c",keycode));
	//ExecuteEvent();
}

void input::ExecuteEvent()
{
	EventQueue.front().OperateFunc();
}

