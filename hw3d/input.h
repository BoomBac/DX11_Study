#pragma once
#include <queue>
#include <map>

typedef void (*OperateBind) ();
enum class KeyState
{
	Press,
	Release
};
enum class MouseState
{
	LPress,
	LRelease,
	RPress,
	RRelease,
	WheelUp,
	WheelDown,
	Move
};


class input
{
	//键盘事件类
	class KEvent
	{
	public:
		KEvent(unsigned char keycode, KeyState keystate, OperateBind Op);
		void (*m_OperateFunc)();
		KeyState m_Keystate;
		unsigned char m_KeyCode;
	};
	//鼠标事件类
	class MEvent
	{
	public:
		MEvent(MouseState mousestate, OperateBind op);
		void (*m_OperateFunc)();
		MouseState m_MouseState;
	};

private:
	// 从windw获取的键盘事件队列
	std::queue<KEvent> KEventQueue;
	//存储键盘按下对应操作
	std::map<unsigned char, OperateBind> KeyPressOp;
	//存储键盘松开对应操作
	std::map<unsigned char, OperateBind> KeyReleaseOp;


	// 从windw获取的鼠标事件队列
	std::queue<MEvent> MEventQueue;
	
	//存储鼠标状态对应操作
	std::map<MouseState, OperateBind> MouseOp;

public:
	//鼠标坐标
	static int m_x;
	static int m_y;
	input() = default;

	//填充键盘操作映射表
	void AddKeyBind(unsigned char keycode,OperateBind Bop, KeyState keystate);
	//从window获取键盘信息，参一为键码，参二为事件：按下/松开
	void OnKeyCaptured(unsigned char keycode,KeyState keystate);

	//填充鼠标操作映射表
	void AddMouseBind(MouseState mousestate, OperateBind Bop);
	//从window获取鼠标信息
	void OnMouseCapture(MouseState mousestate, int x, int y);
	//鼠标默认操作
   static void MDefaultOp();

	//依次执行队列事件
	void ExecuteEvent();
};

