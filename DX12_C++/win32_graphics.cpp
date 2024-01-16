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


void DrawTriangle(
	v3 ModelVertex0, v3 ModelVertex1, v3 ModelVertex2,
	v3 ModelColor0, v3 ModelColor1, v3 ModelColor2,
	m4 Transform)
{
	v3 TransformedPoint0 = (Transform * V4(ModelVertex0, 1.0f)).xyz;
	v3 TransformedPoint1 = (Transform * V4(ModelVertex1, 1.0f)).xyz;
	v3 TransformedPoint2 = (Transform * V4(ModelVertex2, 1.0f)).xyz;

	v2 PointA = ProjetPoint(TransformedPoint0);
	v2 PointB = ProjetPoint(TransformedPoint1);
	v2 PointC = ProjetPoint(TransformedPoint2);

	//��� ���������� �� ��������� ����� ������ ����� � �� �� � ��� �����
	//��������� ��� ���������� ��������� ����� �� ���� ����� ����� �������������� ��������� �� �������� �����, ������� ����������� ������� � ������� ������ ���� ���� �������� �����
	i32 MinX = min(min((i32)PointA.x, (i32)PointB.x), (i32)PointC.x);
	i32 MaxX = max(max((i32)round(PointA.x), (i32)round(PointB.x)), (i32)round(PointC.x));
	i32 MinY = min(min((i32)PointA.y, (i32)PointB.y), (i32)PointC.y);
	i32 MaxY = max(max((i32)round(PointA.y), (i32)round(PointB.y)), (i32)round(PointC.y));

	//�������� �� ��������� � ������
	MinX = max(0, MinX);                                                              //�������� �� ��������� �������� ���������� ���� � ��� �������
	MinX = min(GlobalState.FrameBufferWidth - 1, MinX);                              //�������� �� ��������� �������� ���������� ���� � ����� �������
	MaxX = max(0, MaxX);                                                            //�������� �� �������� �������� ���������� ���� � ��� �������
	MaxX = min(GlobalState.FrameBufferWidth - 1, MaxX);                            //�������� �� �������� �������� ���������� ���� � ����� �������

	MinY = max(0, MinY);                                                         //�������� �� ������ ������� ���������� ���� � ������ �������
	MinY = min(GlobalState.FrameBufferHeight - 1, MinY);                        //�������� �� ������ ������� ���������� ���� � ������� �������
	MaxY = max(0, MaxY);                                                       //�������� �� ������ ������� ���������� ���� � ������ �������
	MaxY = min(GlobalState.FrameBufferHeight - 1, MaxY);                      //�������� �� ������ ������� ���������� ���� � ������� �������


	//��������� ������� ����������
	v2 Edge0 = PointB - PointA;
	v2 Edge1 = PointC - PointB;
	v2 Edge2 = PointA - PointC;

	//�������� ������� ����� �� ���� � ��������
	b32 IsTopLeft0 = (Edge0.y > 0.0f) || (Edge0.x > 0.0f && Edge0.y == 0.0f);
	b32 IsTopLeft1 = (Edge1.y > 0.0f) || (Edge1.x > 0.0f && Edge1.y == 0.0f);
	b32 IsTopLeft2 = (Edge2.y > 0.0f) || (Edge2.x > 0.0f && Edge2.y == 0.0f);


	f32 BarryCentricDiv = CrossProduct2d(PointB - PointA, PointC - PointA);


	for (i32 Y = MinY; Y <= MaxY; ++Y)
	{
		for (i32 X = MinX; X <= MaxX; ++X)
		{
			v2 PixelPoint = V2((f32)X, (f32)Y) + V2(0.5f, 0.5f);

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
				u32 PixelId = Y * GlobalState.FrameBufferWidth + X;

				//���������� ������������ ����������
				f32 T0 = -CrossLenght1 / BarryCentricDiv;
				f32 T1 = -CrossLenght2 / BarryCentricDiv;
				f32 T2 = -CrossLenght0 / BarryCentricDiv;

				//������� ��� ���������� ������������
				f32 Depth = 1.0f / (T0 * (1.0f / TransformedPoint0.z) + T1 * (1.0f / TransformedPoint1.z) + T2 * (1.0f / TransformedPoint2.z));
				if (Depth < GlobalState.DepthBuffer[PixelId]) 
				{
					//������������ ������� � ������ ��������� � �����
					v3 FinalColor = T0 * ModelColor0 + T1 * ModelColor1 + T2 * ModelColor2;
					FinalColor = FinalColor * 255.f;
					u32 FinalColorU32 = ((u32)0xFF << 24) | ((u32)FinalColor.r << 16) | ((u32)FinalColor.g << 8) | (u32)FinalColor.b;

					GlobalState.FrameBufferPixels[PixelId] = FinalColorU32;                                                      //�������� ���� � ����� �����
					GlobalState.DepthBuffer[PixelId] = Depth;                                                                 //�������� ������� � ����� �������
				}
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
		GlobalState.FrameBufferPixels = (u32*)malloc(sizeof(u32) * GlobalState.FrameBufferWidth * GlobalState.FrameBufferHeight);  //�������� ���'�� ������ �����
		GlobalState.DepthBuffer = (f32*)malloc(sizeof(f32) * GlobalState.FrameBufferWidth * GlobalState.FrameBufferHeight);       //�������� ���'�� ������ �������
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

			//ֳ ������ ���������� �� ����� ������
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
				u32 VkCode = Message.wParam;                                       //������� ��������� ���������� ������ � �������� ��������� � ����� ���� ������ � ASCII ������
				b32 IsDown = !((Message.lParam >> 31) & 0x1);                              //�� ������ ������ 

				switch (VkCode)
				{
				case 'W':
				{
					GlobalState.WDown = IsDown;
				} break;

				case 'A':
				{
					GlobalState.ADown = IsDown;

				} break;

				case 'S':
				{
					GlobalState.SDown = IsDown;

				} break;

				case 'D':
				{
					GlobalState.DDown = IsDown;

				} break;

				case VK_SPACE:
				{
					GlobalState.SpaceDown = IsDown;

				} break;

				case VK_SHIFT:
				{
					GlobalState.ShiftDown = IsDown;

				} break;

				default:
				{
				} break;
				}
			} break;

			default: {
				TranslateMessage(&Message);                        //������������� ����������
				DispatchMessage(&Message);                        //���������� �� � ����������� �� ������������
			} break;
			}
		}


		f32 Speed = 0.4f;
		GlobalState.CurrTime += Speed * FrameTime;


		for (u32 Y = 0; Y < GlobalState.FrameBufferHeight; ++Y)
		{
			for (u32 X = 0; X < GlobalState.FrameBufferWidth; ++X)
			{
				u32 PixelId = Y * GlobalState.FrameBufferWidth + X;                     //������� ����������� ������ � ���'��

				u8 Red = 00;                                                          //���������� ���� ������
				u8 Green = 00;
				u8 Blue = 00;
				u8 Alfa = 255;
				u32 PixelColor = ((u32)Alfa << 24) | ((u32)Red << 16) | ((u32)Green << 8) | (u32)Blue;

				GlobalState.FrameBufferPixels[PixelId] = PixelColor;            //���� ���� ���� ������ ARGB
				GlobalState.DepthBuffer[PixelId] = FLT_MAX;                    //������� ����� �����


			}
		}

		//�������� ���� ������
		m4 CameraTransform = IdentityM4();
		{
			camera* Camera = &GlobalState.Camera;

			b32 MouseDown = false;
			v2 CurrMousePos = {};
			if (GetActiveWindow() == GlobalState.WindowHandle)                                                //�������� �� ���� ���� �������
			{
				POINT Win32MousePos = {};
				Assert(GetCursorPos(&Win32MousePos));                                                      //�������� ��������� �������
				Assert(ScreenToClient(GlobalState.WindowHandle, &Win32MousePos));                         //�������� ��������� ������� � ������� ����

				RECT ClientRect = {};
				Assert(GetClientRect(GlobalState.WindowHandle, &ClientRect));                          //������������ ��� ��������� ������� �� �������� �� ������ ����
				Win32MousePos.y = ClientRect.bottom - Win32MousePos.y;                                //���������� ���� ��������� �� ����� ���� �� ����

				CurrMousePos.x = f32(Win32MousePos.x) / f32(ClientRect.right - ClientRect.left);
				CurrMousePos.y = f32(Win32MousePos.y) / f32(ClientRect.bottom - ClientRect.top);

				MouseDown = (GetKeyState(VK_LBUTTON) & 0x80) != 0;
			}

			//�������� �� ������ ������
			if (MouseDown)
			{
				if (!Camera->PrevMouseDown)
				{
					Camera->PrevMousePos = CurrMousePos;
				}

				v2 MouseDelta = CurrMousePos - Camera->PrevMousePos;
				Camera->Pitch += MouseDelta.y;
				Camera->Yaw += MouseDelta.x;

				Camera->PrevMousePos = CurrMousePos;
			}

			Camera->PrevMouseDown = MouseDown;

			m4 YawTransform = RotationMatrix(0, Camera->Yaw, 0);
			m4 PitchTransform = RotationMatrix(Camera->Pitch, 0, 0);
			m4 CameraAxisTransform = YawTransform * PitchTransform;

			v3 Right = Normalize((CameraAxisTransform * V4(1, 0, 0, 0)).xyz);
			v3 Up = Normalize((CameraAxisTransform * V4(0, 1, 0, 0)).xyz);;
			v3 LookAt = Normalize((CameraAxisTransform * V4(0, 0, 1, 0)).xyz);;

			m4 CameraViewTransform = IdentityM4();

			CameraViewTransform.v[0].x = Right.x;
			CameraViewTransform.v[1].x = Right.y;
			CameraViewTransform.v[2].x = Right.z;

			CameraViewTransform.v[0].y = Up.x;
			CameraViewTransform.v[1].y = Up.y;
			CameraViewTransform.v[2].y = Up.z;

			CameraViewTransform.v[0].z = LookAt.x;
			CameraViewTransform.v[1].z = LookAt.y;
			CameraViewTransform.v[2].z = LookAt.z;

			if (GlobalState.WDown)
			{
				Camera->Pos += LookAt * FrameTime;
				OutputDebugStringA("WDown\n");
			}
			if (GlobalState.SDown)
			{
				Camera->Pos -= LookAt * FrameTime;
			}
			if (GlobalState.ADown)
			{
				Camera->Pos -= Right * FrameTime;
			}
			if (GlobalState.DDown)
			{
				Camera->Pos += Right * FrameTime;
			}
			if (GlobalState.SpaceDown)
			{
				Camera->Pos += Up * FrameTime;
			}
			if (GlobalState.ShiftDown)
			{
				Camera->Pos -= Up * FrameTime;
			}

			CameraTransform = CameraViewTransform * TranslationMatrix(-Camera->Pos);
		}

		// �������� ���� ����������
		GlobalState.CurrTime = GlobalState.CurrTime + FrameTime;
		if (GlobalState.CurrTime > 2.0f * 3.14159f)
		{
			GlobalState.CurrTime -= 2.0f * 3.14159f;
		}

		v3 ModelVertices[] =
		{
			//������� �������
			V3(-0.5f, -0.5f, -0.5f),
			V3(-0.5f,  0.5f, -0.5f),
			V3( 0.5f,  0.5f, -0.5f),
			V3( 0.5f, -0.5f, -0.5f),

			//����� �������
			V3(-0.5f, -0.5f, 0.5f),
			V3(-0.5f,  0.5f, 0.5f),
			V3(0.5f,  0.5f,  0.5f),
			V3(0.5f, -0.5f,  0.5f),
		};

		v3 ModelColors[] =
		{
			V3(1, 0, 0),
			V3(0, 1, 0),
			V3(0, 0.2, 0.67),                                                                               //UA Blue ����� ����� �������
			V3(1, 0, 1),

			V3(1, 1, 0),
			V3(0, 0.2, 0.67),
			V3(1, 0, 1),
			V3(1, 1, 1),
		};

		u32 ModelIndices[] = 
		{
			//������� �������
			0, 1, 2,
			2, 3, 0,

			//����� �������
			6, 5, 4,
			4, 7, 6,

			//˳�� �������
			4, 5, 1,
			1, 0, 4,

			//����� �������
			3, 2, 6,
			6, 7, 3,

			//������ �������
			1, 5, 6,
			6, 2, 1,

			//����� �������
			4, 0, 3,
			3, 7, 4,
		};


		f32 Offset = abs(sin(GlobalState.CurrTime));

		m4 Transform = (CameraTransform *
			TranslationMatrix(/*x*/0, /*Y*/0, /*Z*/2) *
			RotationMatrix(GlobalState.CurrTime, GlobalState.CurrTime, GlobalState.CurrTime) *
			ScaleMatrix(1, 1, 1));

		for (u32 IndexId = 0; IndexId < ArrayCount(ModelIndices); IndexId += 3 ) 
		{
			u32 Index0 = ModelIndices[IndexId + 0];
			u32 Index1 = ModelIndices[IndexId + 1];
			u32 Index2 = ModelIndices[IndexId + 2];

			DrawTriangle(
				ModelVertices[Index0], ModelVertices[Index1], ModelVertices[Index2],
				ModelColors[Index0], ModelColors[Index1], ModelColors[Index2],
				Transform);
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