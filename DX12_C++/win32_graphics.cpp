#include <Windows.h>
#include "Headers/win32_graphics.h"

global global_state GlobalState;


LRESULT Win32WindowsCallBack(
	HWND WindowHandle,
	UINT Message,
	WPARAM WParam,
	LPARAM Lparam
)
{
	LRESULT Result = {};

	switch (Message)                                               //���������� �� ����������� �� � ����� ����������
	{
		case WM_DESTROY:                                           //
		case WM_CLOSE:
		{
			GlobalState.isRunnig = false;
		} break;
		default:
		{
			Result = DefWindowProcA(WindowHandle, Message, WParam, Lparam);
		} break;
	}
	
	
	
	
	
	//DefWindowProcA ������� �� �� ������������� ���� ����������� ����������� �� �������
	return Result;
}


int APIENTRY WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd) 
{
	GlobalState.isRunnig = true;


	WNDCLASSA WindowClass = {};
	WindowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;          //��� ����� ������� �� ����� ���� ��� �� �������� https://learn.microsoft.com/ru-ru/windows/win32/winmsg/window-class-styles
	WindowClass.lpfnWndProc = Win32WindowsCallBack;                  //�������� �� ���������� �� ������������� �� ������
	WindowClass.hInstance = hInstance;                               //������ ��������
	WindowClass.hCursor = LoadCursorA(NULL, IDC_WAIT);               //������ �������� 
	WindowClass.lpszClassName = "Bonjour come us EVERY BODY SUCK MY DICK";                //��'� ������ �����

	if (!RegisterClassA(&WindowClass)) 
	{
		InvalideCodePatch;
	}



	GlobalState.WindowHandle = CreateWindowExA(
		0,                                                           //��� ����� ��� ������ ��� ���� ����  
		WindowClass.lpszClassName,                                   //��'� ������ ����� �2
		"EVERY BODY SUCK MY DICK",                                   //��'� ������ ����
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,                            //����� ���� ��� WS_VISIBLE ���� ���� �� ���� �� �� �� �������� ���� ���� ������ �� ���� ���� �������� https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles
		CW_USEDEFAULT,                                               //��������� ���� �� �
		CW_USEDEFAULT,                                               //��������� ���� �� Y
		1280,                                                        //����� ���� �� �
		720,                                                         //����� ���� �� Y
		NULL,                                                        //�� ���'����� �� ���� � ������
		NULL,                                                        //�� ���� ���� � ���
		hInstance,
		NULL);

	if (!GlobalState.WindowHandle) 
	{
		InvalideCodePatch;
	}

	while (GlobalState.isRunnig)                                     //�������� ���� ���� �������� �� �������� �������
	{
		MSG Message = {};
		while (PeekMessageA(&Message, GlobalState.WindowHandle, 0, 0, PM_REMOVE))              //���� ��� �������� ����������
		{
			switch (Message.message)                                                           //���� ��� ����� ���� �������� ��� �����������
			{
				case WM_QUIT: 
				{
					GlobalState.isRunnig = false;
				} break;
				default: {
					TranslateMessage(&Message);                       //������������� ����������
					DispatchMessage(&Message);                        //���������� �� � ����������� �� ������������
				} break;
			}
		}
	}


	return 0;
}