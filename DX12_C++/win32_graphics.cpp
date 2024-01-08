#include <cmath>
#include <Windows.h>

#include "win32_graphics.h"
#include "graphics_math.cpp"

global global_state GlobalState;


v2 ProjetPoint(v3 WorldPos)
{
	v2 Result = {};
	// NOTE: ������������ �� NDC
	Result = WorldPos.xy / WorldPos.z;

	// NOTE: ������������ �� ������
	Result = 0.5f * (Result + V2(1.0f, 1.0f));
	Result = V2(GlobalState.FrameBufferWidth, GlobalState.FrameBufferHeight) * Result;

	return Result;
}

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
			//DefWindowProcA ������� �� �� ������������� ���� ����������� ����������� �� �������
			Result = DefWindowProcA(WindowHandle, Message, WParam, Lparam);
		} break;
	}


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
	Assert(QueryPerformanceFrequency(&TimerFrequency));                          //������ �����


	{
		WNDCLASSA WindowClass = {};
		WindowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;             //��� ����� ������� �� ����� ���� ��� �� �������� https://learn.microsoft.com/ru-ru/windows/win32/winmsg/window-class-styles
		WindowClass.lpfnWndProc = Win32WindowsCallBack;                    //�������� �� ���������� �� ������������� �� ������
		WindowClass.hInstance = hInstance;                                //������ ��������
		WindowClass.hCursor = LoadCursorA(NULL, IDC_WAIT);               //������ �������� 
		WindowClass.lpszClassName = "Bonjour";                          //��'� ������ �����

		if (!RegisterClassA(&WindowClass))
		{
			InvalideCodePatch;
		}



		GlobalState.WindowHandle = CreateWindowExA(
			0,                                                                    //��� ����� ��� ������ ��� ���� ����  
			WindowClass.lpszClassName,                                           //��'� ������ ����� �2
			"Graphic Test",                                                     //��'� ������ ����
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,                                  //����� ���� ��� WS_VISIBLE ���� ���� �� ���� �� �� �� �������� ���� ���� ������ �� ���� ���� �������� https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles
			CW_USEDEFAULT,                                                    //��������� ���� �� �
			CW_USEDEFAULT,                                                   //��������� ���� �� Y
			1280,                                                           //����� ���� �� �
			720,                                                           //����� ���� �� Y
			NULL,                                                         //�� ���'����� �� ���� � ������
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
		//GlobalState.FrameBufferWidth = ClientRect.right - ClientRect.left;
		//GlobalState.FrameBufferHeight = ClientRect.bottom - ClientRect.top;
		GlobalState.FrameBufferWidth = 340;
		GlobalState.FrameBufferHeight = 340;
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
					TranslateMessage(&Message);                        //������������� ����������
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
				u32 PixelId = Y * GlobalState.FrameBufferWidth + X;                    //������� ����������� ������ � ���'��

				u8 Red = 00;                                                         //���������� ���� ������
				u8 Green = 00;
				u8 Blue = 00;
				u8 Alfa = 255;
				u32 PixelColor = ((u32)Alfa << 24) | ((u32)Red << 16) | ((u32)Green << 8) | (u32)Blue;

				GlobalState.FrameBufferPixels[PixelId] = PixelColor;            //���� ���� ���� ������ ������ ARGB


			}
		}
		
		for (u32 TriangleId = 0; TriangleId < 5; ++TriangleId)                                      //���������� ������� ����������
		{
			f32 Depth = powf(2, TriangleId + 1);                                                  //���������� ������� ������� ����������
			    
			v3 Points[3] = {
				V3(-1.0f, -0.5f, Depth),                                                       // ˳�� ����� ����������
				V3(1.0f, -0.5f, Depth),                                                       // ����� ����� ����������
				V3(0.0f, 0.5f, Depth),                                                       // ������ ����� ����������
			};

			for (u32 PointId = 0; PointId < ArrayCount(Points); ++PointId)
			{
				v3 TransformedPos = Points[PointId] + V3(cosf(GlobalState.CurrAngle), sinf(GlobalState.CurrAngle), 0);
				v2 PixelPos = ProjetPoint(TransformedPos);

				if (PixelPos.x >= 0.0f && PixelPos.x < GlobalState.FrameBufferWidth &&
					PixelPos.y >= 0.0f && PixelPos.y < GlobalState.FrameBufferHeight)
				{
					u32 PixelId = u32(PixelPos.y) * GlobalState.FrameBufferWidth + u32(PixelPos.x);


					u8 Red = PixelPos.x;                                             //���������� ���� ������ ����������
					u8 Green = PixelPos.y;
					u8 Blue = GlobalState.CurrOffSet;
					u8 Alfa = 255;
					u32 PixelColor = ((u32)Alfa << 24) | ((u32)Red << 16) | ((u32)Green << 8) | (u32)Blue;
					GlobalState.FrameBufferPixels[PixelId] = PixelColor;
				}
			}
		}

		GlobalState.CurrAngle += FrameTime;
		if (GlobalState.CurrAngle >= 2.0f * Pi32) 
		{
			GlobalState.CurrAngle -= 2.0f * Pi32;
		}

		
		RECT ClientRect = {};                                                       //ĳ������� ����� ������ �����
		Assert(GetClientRect(GlobalState.WindowHandle, &ClientRect));
		u32 ClientWidth = ClientRect.right - ClientRect.left;
		u32 ClientHeight = ClientRect.bottom - ClientRect.top;
		

		BITMAPINFO BitMapInfo = {};
		BitMapInfo.bmiHeader.biSize = sizeof(tagBITMAPINFOHEADER);
		BitMapInfo.bmiHeader.biWidth = GlobalState.FrameBufferWidth;
		BitMapInfo.bmiHeader.biHeight = GlobalState.FrameBufferHeight;
		BitMapInfo.bmiHeader.biPlanes = 1;                                                    //�� ����� ������� ���� ������ 1
		BitMapInfo.bmiHeader.biBitCount = 32;
		BitMapInfo.bmiHeader.biCompression = BI_RGB;

		//������� ����� �� ����
		Assert(StretchDIBits(
			GlobalState.DeviceContext,
			0,                                                                           //�� ��������� ����������� �� ����� ������� � ��� �� �
			0,                                                                          //�� ��������� ����������� �� ����� ������� � ��� �� Y
			ClientWidth,                                                               //����� �볺������ ����� ������
			ClientHeight,                                                             //����� �볺������ ����� ������
			0,                                                                       //��������� ���������� � ����� �� �
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