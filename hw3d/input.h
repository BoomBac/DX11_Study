#pragma once
#include <queue>
#include <map>

typedef void (*OperateBind) ();
enum class KeyState
{
	Press,
	Release
};

class input
{
	//键盘事件类
	class Event
	{
	public:
		Event(unsigned char keycode, KeyState keystate, OperateBind Op);
		void (*OperateFunc)();
		KeyState Keystate;
		unsigned char KeyCode;
	};

private:
	// 从windw获取的键盘事件队列
	std::queue<Event> EventQueue;
	//存储键盘按下对应操作
	std::map<unsigned char, OperateBind> KeyPressOp;
	//存储键盘松开对应操作
	std::map<unsigned char, OperateBind> KeyReleaseOp;

public:

	//填充操作映射表
	void AddKeyBind(unsigned char keycode,OperateBind Bop, KeyState keystate);
	//从window获取键盘信息，参一为键码，参二为事件：按下/松开
	void OnKeyCaptured(unsigned char keycode,KeyState keystate);

	//依次执行队列事件
	void ExecuteEvent();
};

