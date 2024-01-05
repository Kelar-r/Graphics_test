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


	WNDCLASSA WindowClass = {};
	WindowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;          //Цей рядок відповідає за стиль вікна ось ці парапори https://learn.microsoft.com/ru-ru/windows/win32/winmsg/window-class-styles
	WindowClass.lpfnWndProc = Win32WindowsCallBack;                  //Вказівник до повідомлень що приходитимуть від віндовс
	WindowClass.hInstance = hInstance;                               //Зразок програми
	WindowClass.hCursor = LoadCursorA(NULL, IDC_WAIT);               //Курсор Програми 
	WindowClass.lpszClassName = "Bonjour come us EVERY BODY SUCK MY DICK";                //ім'я нашого класу

	if (!RegisterClassA(&WindowClass)) 
	{
		InvalideCodePatch;
	}



	GlobalState.WindowHandle = CreateWindowExA(
		0,                                                           //Тут можна теж водити різні стилі вікна  
		WindowClass.lpszClassName,                                   //Ім'я нашого класу Х2
		"EVERY BODY SUCK MY DICK",                                   //Ім'я нашого вікна
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

	while (GlobalState.isRunnig)                                     //Головний цикл який перевіряє чи програма закрита
	{
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
	}


	return 0;
}