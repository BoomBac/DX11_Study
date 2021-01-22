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
	//�����¼���
	class Event
	{
	public:
		Event(unsigned char keycode, KeyState keystate, OperateBind Op);
		void (*OperateFunc)();
		KeyState Keystate;
		unsigned char KeyCode;
	};

private:
	// ��windw��ȡ�ļ����¼�����
	std::queue<Event> EventQueue;
	//�洢���̰��¶�Ӧ����
	std::map<unsigned char, OperateBind> KeyPressOp;
	//�洢�����ɿ���Ӧ����
	std::map<unsigned char, OperateBind> KeyReleaseOp;

public:

	//������ӳ���
	void AddKeyBind(unsigned char keycode,OperateBind Bop, KeyState keystate);
	//��window��ȡ������Ϣ����һΪ���룬�ζ�Ϊ�¼�������/�ɿ�
	void OnKeyCaptured(unsigned char keycode,KeyState keystate);

	//����ִ�ж����¼�
	void ExecuteEvent();
};

