#include "input.h"
#include "Tool.h"

using namespace std;


// 默认操作函数
void DefaultOp()
{
	OutputDebugString(TEXT("KeyBoardDebug"));
}
void input::MDefaultOp()
{
	WCHAR output[64];
	swprintf(output, 64, L"x pos is %d,y pos is %d\n", m_x, m_y);
	OutputDebugString(output);
}
input::KEvent::KEvent(unsigned char keycode, KeyState keystate, OperateBind Op) : m_KeyCode(keycode), m_Keystate(keystate), m_OperateFunc(Op)
{
	if (Op == nullptr)
		m_OperateFunc = DefaultOp;
}



int input::m_x;
int input::m_y;

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
			KEventQueue.push(input::KEvent(keycode, KeyState::Press, nullptr));
		}
		else
		{
			KEventQueue.push(input::KEvent(keycode, KeyState::Press, KeyPressOp.find(keycode)->second));
		}
	}
	if (keystate == KeyState::Release)
	{
		if (KeyPressOp.size() == 0)
		{
			KEventQueue.push(input::KEvent(keycode, KeyState::Release, nullptr));
		}
		else
		{
			KEventQueue.push(input::KEvent(keycode, KeyState::Release, KeyReleaseOp.find(keycode)->second));
		}
	}
	//测试用
	//OutputDebugString(TEXT("%c",keycode));
	ExecuteEvent();
}

input::MEvent::MEvent(MouseState mousestate, OperateBind op) : m_MouseState(mousestate), m_OperateFunc(op) {};

void input::AddMouseBind(MouseState mousestate, OperateBind Bop)
{
	MouseOp.insert(pair<MouseState, OperateBind>(mousestate, Bop));
}

void input::OnMouseCapture(MouseState mousestate, int x, int y)
{
	m_x = x;
	m_y = y;
	MEvent T_event(mousestate, MDefaultOp);
		if (MouseOp.size() != 0)
		{
			T_event.m_OperateFunc = MouseOp.find(mousestate)->second;
		}
		MEventQueue.push(T_event);

		ExecuteEvent();
}




void input::ExecuteEvent()
{
	while (!KEventQueue.empty())
	{
		KEventQueue.front().m_OperateFunc();
		KEventQueue.pop();
	}
	while (!MEventQueue.empty())
	{
		MEventQueue.front().m_OperateFunc();
		MEventQueue.pop();
	}
}