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


f32 CrossProduct2d(v2 A, v2 B)
{
	f32 Result = A.x * B.y - A.y * B.x;                     //������� ����� ���������� �� A �� ��������� ������ � � �����
	return Result;
}


void DrawTriangle(v3* Points, u32 Color) 
{
	v2 PointA = ProjetPoint(Points[0]);
	v2 PointB = ProjetPoint(Points[1]);
	v2 PointC = ProjetPoint(Points[2]);

	//��������� ������� ����������
	v2 Edge0 = PointB - PointA;
	v2 Edge1 = PointC - PointB;
	v2 Edge2 = PointA - PointC;

	//�������� ������� ����� �� ���� � ��������
	b32 IsTopLeft0 = (Edge0.y > 0.0f) || (Edge0.x > 0.0f && Edge0.y == 0.0f);
	b32 IsTopLeft1 = (Edge1.y > 0.0f) || (Edge1.x > 0.0f && Edge1.y == 0.0f);
	b32 IsTopLeft2 = (Edge2.y > 0.0f) || (Edge2.x > 0.0f && Edge2.y == 0.0f);


	for (u32 Y = 0; Y < GlobalState.FrameBufferHeight; ++Y)
	{
		for (u32 X = 0; X < GlobalState.FrameBufferWidth; ++X)
		{
			v2 PixelPoint = V2(X, Y) + V2(0.5f, 0.5f);

			v2 PixelEdge0 = PixelPoint - PointA;
			v2 PixelEdge1 = PixelPoint - PointB;
			v2 PixelEdge2 = PixelPoint - PointC;

			//�������� ��������� ������� ����� ��������
			f32 CrossLenght0 = CrossProduct2d(PixelEdge0, Edge0);
			f32 CrossLenght1 = CrossProduct2d(PixelEdge1, Edge1);
			f32 CrossLenght2 = CrossProduct2d(PixelEdge2, Edge2);

			//�������� �� �� � ������� ����������
			if ((CrossLenght0 > 0.0f || (IsTopLeft0 && CrossLenght0 == 0.0f)) &&
				(CrossLenght1 > 0.0f || (IsTopLeft1 && CrossLenght1 == 0.0f)) &&
				(CrossLenght2 > 0.0f || (IsTopLeft2 && CrossLenght2 == 0.0f)))
			{
				// �� �������� ����������
				u32 PixelId = Y * GlobalState.FrameBufferWidth + X;
				GlobalState.FrameBufferPixels[PixelId] = Color;
			}
		}
	}
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
		GlobalState.FrameBufferWidth = 350;
		GlobalState.FrameBufferHeight = 350;
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

				GlobalState.FrameBufferPixels[PixelId] = PixelColor;            //���� ���� ���� ������ ARGB


			}
		}
		
		
		//�������� ���� ����������
		GlobalState.CurrAngle += FrameTime;
		if (GlobalState.CurrAngle >= 2.0f * Pi32)
		{
			GlobalState.CurrAngle -= 2.0f * Pi32;
		}

		u32 Colors[] = 
		{
			0xFF0033AA,                                                                              //UA Blue ����� ����� �������
			0xFF1D2951,
			0xFF3F00FF,
			0xFF87CEEB,
			0xFF483D8B,
		};


		for (i32 TriangleId = 9; TriangleId >= 0; --TriangleId)                                      //���������� ������� ����������
		{
			f32 Depth = powf(2, TriangleId + 1);                                                  //���������� ������� ������� ����������
			    
			v3 Points[3] = {
				V3(-1.0f, -0.5f, Depth),                                                       // ˳�� ����� ����������   X Y Z
				V3(0.0f, 0.5f, Depth),                                                        // ������ ����� ����������   X Y Z
				V3(1.0f, -0.5f, Depth),                                                      // ����� ����� ����������   X Y Z
			};

			
			for (u32 PointId = 0; PointId < ArrayCount(Points); ++PointId)               //������ ����� ���������� � ����
			{
				v3 TransformedPos = Points[PointId] + V3(cosf(GlobalState.CurrAngle), sinf(GlobalState.CurrAngle), 0);
				Points[PointId] = TransformedPos;
			}
			
			DrawTriangle(Points, Colors[TriangleId % (ArrayCount(Colors))]);
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