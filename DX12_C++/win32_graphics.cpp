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
	LARGE_INTEGER TimerFrequency = {};
	Assert(QueryPerformanceFrequency(&TimerFrequency));                  //������ �����


	{
		WNDCLASSA WindowClass = {};
		WindowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;          //��� ����� ������� �� ����� ���� ��� �� �������� https://learn.microsoft.com/ru-ru/windows/win32/winmsg/window-class-styles
		WindowClass.lpfnWndProc = Win32WindowsCallBack;                  //�������� �� ���������� �� ������������� �� ������
		WindowClass.hInstance = hInstance;                               //������ ��������
		WindowClass.hCursor = LoadCursorA(NULL, IDC_WAIT);               //������ �������� 
		WindowClass.lpszClassName = "Bonjour";                //��'� ������ �����

		if (!RegisterClassA(&WindowClass))
		{
			InvalideCodePatch;
		}



		GlobalState.WindowHandle = CreateWindowExA(
			0,                                                           //��� ����� ��� ������ ��� ���� ����  
			WindowClass.lpszClassName,                                   //��'� ������ ����� �2
			"Graphic Test",                                   //��'� ������ ����
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

		GlobalState.DeviceContext = GetDC(GlobalState.WindowHandle);
	}


	{
		RECT ClientRect = {};
		Assert(GetClientRect(GlobalState.WindowHandle, &ClientRect));
		GlobalState.FrameBufferWidth = ClientRect.right - ClientRect.left;
		GlobalState.FrameBufferHeight = ClientRect.bottom - ClientRect.top;
		GlobalState.FrameBufferPixels = (u32*)malloc(sizeof(u32) * GlobalState.FrameBufferWidth * GlobalState.FrameBufferHeight);  //�������� ���'��
	}
	
	LARGE_INTEGER BeginTime = {};                                    //���������� ��� �����
	LARGE_INTEGER EndTime = {};                                      //ʳ������ ��� �����
	Assert(QueryPerformanceCounter(&BeginTime));                    


	while (GlobalState.isRunnig)                                     //�������� ���� ���� �������� �� �������� �������
	{
		Assert(QueryPerformanceCounter(&EndTime));
		f32 FrameTime = f32(EndTime.QuadPart - BeginTime.QuadPart) / f32(TimerFrequency.QuadPart);             //������� ��� ���������� ���� �����
		BeginTime = EndTime;


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


		f32 Speed = 200.0f;
		GlobalState.CurrOffSet += Speed * FrameTime;


		for (u32 Y = 0; Y < GlobalState.FrameBufferHeight; ++Y) 
		{
			for (u32 X = 0; X < GlobalState.FrameBufferWidth; ++X)
			{
				u32 PixelId = Y * GlobalState.FrameBufferWidth + X;                 //������� ����������� ������ � ���'��

				u8 Red = (u8)(X + GlobalState.CurrOffSet);                          //���������� ���� ������
				u8 Green = (u8)(Y + GlobalState.CurrOffSet);
				u8 Blue = GlobalState.CurrOffSet;
				u8 Alfa = 255;
				u32 PixelColor = ((u32)Alfa << 24) | ((u32)Red << 16) | ((u32)Green << 8) | (u32)Blue;

				GlobalState.FrameBufferPixels[PixelId] = PixelColor;                //���� ���� ���� ������ ������ ARGB


			}
		}


		
		RECT ClientRect = {};                                                  //ĳ������� ����� ������ �����
		Assert(GetClientRect(GlobalState.WindowHandle, &ClientRect));
		u32 ClientWidth = ClientRect.right - ClientRect.left;
		u32 ClientHeight = ClientRect.bottom - ClientRect.top;
		

		BITMAPINFO BitMapInfo = {};
		BitMapInfo.bmiHeader.biSize = sizeof(tagBITMAPINFOHEADER);
		BitMapInfo.bmiHeader.biWidth = GlobalState.FrameBufferWidth;
		BitMapInfo.bmiHeader.biHeight = GlobalState.FrameBufferHeight;
		BitMapInfo.bmiHeader.biPlanes = 1;                                         //�� ����� ������� ���� ������ 1
		BitMapInfo.bmiHeader.biBitCount = 32;
		BitMapInfo.bmiHeader.biCompression = BI_RGB;

		//������� ����� �� ����
		Assert(StretchDIBits(
			GlobalState.DeviceContext,
			0,                                                                      //�� ��������� ����������� �� ����� ������� � ��� �� �
			0,                                                                      //�� ��������� ����������� �� ����� ������� � ��� �� Y
			ClientWidth,                                                            //����� �볺������ ����� ������
			ClientHeight,                                                           //����� �볺������ ����� ������
			0,                                                                      //��������� ���������� � ����� �� �
			0,                                                                      //��������� ���������� � ����� �� Y
			GlobalState.FrameBufferWidth,
			GlobalState.FrameBufferHeight,
			GlobalState.FrameBufferPixels,
			&BitMapInfo,
			DIB_RGB_COLORS,
			SRCCOPY));
	}


	return 0;
}