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

	switch (Message)                                               //Реагування на повідомлення не з черги повідомлень
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
	
	
	
	
	
	//DefWindowProcA Функція що за замовчуванням буде оброблювати повідомлення від системи
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
	Assert(QueryPerformanceFrequency(&TimerFrequency));                  //Щетчик кадрів


	{
		WNDCLASSA WindowClass = {};
		WindowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;          //Цей рядок відповідає за стиль вікна ось ці парапори https://learn.microsoft.com/ru-ru/windows/win32/winmsg/window-class-styles
		WindowClass.lpfnWndProc = Win32WindowsCallBack;                  //Вказівник до повідомлень що приходитимуть від віндовс
		WindowClass.hInstance = hInstance;                               //Зразок програми
		WindowClass.hCursor = LoadCursorA(NULL, IDC_WAIT);               //Курсор Програми 
		WindowClass.lpszClassName = "Bonjour";                //ім'я нашого класу

		if (!RegisterClassA(&WindowClass))
		{
			InvalideCodePatch;
		}



		GlobalState.WindowHandle = CreateWindowExA(
			0,                                                           //Тут можна теж водити різні стилі вікна  
			WindowClass.lpszClassName,                                   //Ім'я нашого класу Х2
			"Graphic Test",                                   //Ім'я нашого вікна
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,                            //Знову стилі але WS_VISIBLE якщо його не буде то ми не побачимо наше вікно взагалі бо воно буде невидиме https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles
			CW_USEDEFAULT,                                               //Положення вікна по Х
			CW_USEDEFAULT,                                               //Положення вікна по Y
			1280,                                                        //Розмір вікна по Х
			720,                                                         //Розмір вікна по Y
			NULL,                                                        //Чи Пов'язане це вікно з іншими
			NULL,                                                        //Чи буде меню в вікні
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
		GlobalState.FrameBufferPixels = (u32*)malloc(sizeof(u32) * GlobalState.FrameBufferWidth * GlobalState.FrameBufferHeight);  //Виділення пам'яті
	}
	
	LARGE_INTEGER BeginTime = {};                                    //Початковий час кадру
	LARGE_INTEGER EndTime = {};                                      //Кінцевий час кадру
	Assert(QueryPerformanceCounter(&BeginTime));                    


	while (GlobalState.isRunnig)                                     //Головний цикл який перевіряє чи програма закрита
	{
		Assert(QueryPerformanceCounter(&EndTime));
		f32 FrameTime = f32(EndTime.QuadPart - BeginTime.QuadPart) / f32(TimerFrequency.QuadPart);             //Формула для обчислення часу кадра
		BeginTime = EndTime;


		MSG Message = {};
		while (PeekMessageA(&Message, GlobalState.WindowHandle, 0, 0, PM_REMOVE))              //Цикл для перевірки повідомлень
		{
			switch (Message.message)                                                           //Світч щоб можна було розібрати різні повідомлення
			{
				case WM_QUIT: 
				{
					GlobalState.isRunnig = false;
				} break;
				default: {
					TranslateMessage(&Message);                       //Розшифрування повідомлень
					DispatchMessage(&Message);                        //реагування на ті повідомлення по замовчуванню
				} break;
			}
		}


		f32 Speed = 200.0f;
		GlobalState.CurrOffSet += Speed * FrameTime;


		for (u32 Y = 0; Y < GlobalState.FrameBufferHeight; ++Y) 
		{
			for (u32 X = 0; X < GlobalState.FrameBufferWidth; ++X)
			{
				u32 PixelId = Y * GlobalState.FrameBufferWidth + X;                 //Формула обчисленняя пікселя в пам'яті

				u8 Red = (u8)(X + GlobalState.CurrOffSet);                          //Обчислюємо Колір пікселя
				u8 Green = (u8)(Y + GlobalState.CurrOffSet);
				u8 Blue = GlobalState.CurrOffSet;
				u8 Alfa = 255;
				u32 PixelColor = ((u32)Alfa << 24) | ((u32)Red << 16) | ((u32)Green << 8) | (u32)Blue;

				GlobalState.FrameBufferPixels[PixelId] = PixelColor;                //Даємо колір тому пікселю Патерн ARGB


			}
		}


		
		RECT ClientRect = {};                                                  //Дізнаємось розмір робочої площі
		Assert(GetClientRect(GlobalState.WindowHandle, &ClientRect));
		u32 ClientWidth = ClientRect.right - ClientRect.left;
		u32 ClientHeight = ClientRect.bottom - ClientRect.top;
		

		BITMAPINFO BitMapInfo = {};
		BitMapInfo.bmiHeader.biSize = sizeof(tagBITMAPINFOHEADER);
		BitMapInfo.bmiHeader.biWidth = GlobalState.FrameBufferWidth;
		BitMapInfo.bmiHeader.biHeight = GlobalState.FrameBufferHeight;
		BitMapInfo.bmiHeader.biPlanes = 1;                                         //Це лайно повинно бути завжди 1
		BitMapInfo.bmiHeader.biBitCount = 32;
		BitMapInfo.bmiHeader.biCompression = BI_RGB;

		//Копіюємо буфер до вікна
		Assert(StretchDIBits(
			GlobalState.DeviceContext,
			0,                                                                      //Де положений Прямокутник до якого копіюємо у вікні по Х
			0,                                                                      //Де положений Прямокутник до якого копіюємо у вікні по Y
			ClientWidth,                                                            //Розмір клієнтської площі ширина
			ClientHeight,                                                           //Розмір клієнтської площі висота
			0,                                                                      //Положення трикутника в Буфері по Х
			0,                                                                      //Положення трикутника в Буфері по Y
			GlobalState.FrameBufferWidth,
			GlobalState.FrameBufferHeight,
			GlobalState.FrameBufferPixels,
			&BitMapInfo,
			DIB_RGB_COLORS,
			SRCCOPY));
	}


	return 0;
}