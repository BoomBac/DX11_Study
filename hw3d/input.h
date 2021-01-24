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
	//�����¼���
	class KEvent
	{
	public:
		KEvent(unsigned char keycode, KeyState keystate, OperateBind Op);
		void (*m_OperateFunc)();
		KeyState m_Keystate;
		unsigned char m_KeyCode;
	};
	//����¼���
	class MEvent
	{
	public:
		MEvent(MouseState mousestate, OperateBind op);
		void (*m_OperateFunc)();
		MouseState m_MouseState;
	};

private:
	// ��windw��ȡ�ļ����¼�����
	std::queue<KEvent> KEventQueue;
	//�洢���̰��¶�Ӧ����
	std::map<unsigned char, OperateBind> KeyPressOp;
	//�洢�����ɿ���Ӧ����
	std::map<unsigned char, OperateBind> KeyReleaseOp;


	// ��windw��ȡ������¼�����
	std::queue<MEvent> MEventQueue;
	
	//�洢���״̬��Ӧ����
	std::map<MouseState, OperateBind> MouseOp;

public:
	//�������
	static int m_x;
	static int m_y;
	input() = default;

	//�����̲���ӳ���
	void AddKeyBind(unsigned char keycode,OperateBind Bop, KeyState keystate);
	//��window��ȡ������Ϣ����һΪ���룬�ζ�Ϊ�¼�������/�ɿ�
	void OnKeyCaptured(unsigned char keycode,KeyState keystate);

	//���������ӳ���
	void AddMouseBind(MouseState mousestate, OperateBind Bop);
	//��window��ȡ�����Ϣ
	void OnMouseCapture(MouseState mousestate, int x, int y);
	//���Ĭ�ϲ���
   static void MDefaultOp();

	//����ִ�ж����¼�
	void ExecuteEvent();
};

