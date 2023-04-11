/*Програма запускає на виконання 3 потоки.
Перший з них виводить на екран у циклі числа від 1 до 10000,
другий від 10000 до 1,
третій – різницю між числами в першому і в другому потоках.
Після кожного висновку потік припиняється на 10 мс.
Крім цього, на екран кожні 2 сек.виводиться інформація про час виконання кожного потоку.*/

#include <windows.h> // підключення бібліотеки з функціями API
#include <string> 
const UINT WM_APP_MY_THREAD_UPDATE = WM_APP + 0;

// Глобальні змінні:
HINSTANCE hInst; 	//Дескриптор програми	
LPCTSTR szWindowClass = "var2";
LPCTSTR szTitle = "IPP_Lab1";
int x = 1;
int y = 10000;
LARGE_INTEGER Fraq;
LARGE_INTEGER StInc, EnInc, StDec, EnDec, StSub, EnSub;
// Попередній опис функцій
DWORD PrintIncrease(PVOID p);
DWORD PrintDecrease(PVOID p);
DWORD PrintSubtraction(PVOID p);
void RetreaveTime(PVOID p);

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static void Paint(HWND hWnd, LPPAINTSTRUCT lpPS);

// Основна програма 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
	int nCmdShow)
{
	MSG msg;

	// Реєстрація класу вікна 
	MyRegisterClass(hInstance);

	// Створення вікна програми
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	// Цикл обробки повідомлень
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW; 		//стиль вікна
	wcex.lpfnWndProc = (WNDPROC)WndProc; 		//віконна процедура
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance; 			//дескриптор програми
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); 		//визначення іконки
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); 	//визначення курсору
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW); //установка фону
	wcex.lpszMenuName = NULL; 				//визначення меню
	wcex.lpszClassName = szWindowClass; 		//ім’я класу
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex); 			//реєстрація класу вікна
}

// FUNCTION: InitInstance (HANDLE, int)
// Створює вікно програми і зберігає дескриптор програми в змінній hInst

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; //зберігає дескриптор додатка в змінній hInst
	hWnd = CreateWindow(szWindowClass, 	// ім’я класу вікна
		szTitle, 				// назва програми
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZE,			// стиль вікна
		CW_USEDEFAULT, 			// положення по Х	
		CW_USEDEFAULT,			// положення по Y	
		500, 			// розмір по Х
		200, 			// розмір по Y
		NULL, 					// дескриптор батьківського вікна	
		NULL, 					// дескриптор меню вікна
		hInstance, 				// дескриптор програми
		NULL); 				// параметри створення.

	if (!hWnd) 	//Якщо вікно не творилось, функція повертає FALSE
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow); 		//Показати вікно
	UpdateWindow(hWnd); 				//Оновити вікно
	return TRUE;
}

// FUNCTION: WndProc (HWND, unsigned, WORD, LONG)
// Віконна процедура. Приймає і обробляє всі повідомлення, що приходять в додаток

HANDLE threadInc, threadDec, threadSub, timeThread;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt;
	DWORD id;
	switch (message)
	{
	case WM_CREATE: {			//Повідомлення приходить при створенні вікна
		if (QueryPerformanceFrequency(&Fraq) == 0) {
			MessageBox(hWnd, "Встановлене обладнання НЕ підтримує лічильник продуктивності високої роздільної здатності\nЗначення часу може бути не точним",
				"Попередження", MB_ICONWARNING);
		}

		threadInc = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PrintIncrease, hWnd, 0, NULL);
		threadDec = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PrintDecrease, hWnd, 0, NULL);
		threadSub = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PrintSubtraction, hWnd, 0, NULL);
		timeThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RetreaveTime, hWnd, 0, NULL);
		break;
	}

	case WM_PAINT: 				//Перемалювати вікно
		hdc = BeginPaint(hWnd, &ps); 	//Почати графічний вивід	
		Paint(hWnd, &ps);
		EndPaint(hWnd, &ps); 		//Закінчити графічний вивід	
		break;
	case WM_APP_MY_THREAD_UPDATE: {
		GetClientRect(hWnd, &rt);
		rt.left = 0;
		rt.top = lParam - 20; //from TextOut

		rt.bottom = lParam;
		InvalidateRect(hWnd, &rt, TRUE);
		break;
	}

	case WM_DESTROY: 				//Завершення роботи
		PostQuitMessage(0);
		break;
	default:
		//Обробка повідомлень, які не оброблені користувачем
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

DWORD PrintIncrease(PVOID p) {
	QueryPerformanceCounter(&StInc);
	/******************/
	HWND hWnd = (HWND)p;
	HDC hdc = GetDC(hWnd);
	RECT rt;
	GetClientRect(hWnd, &rt);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RetreaveTime, hWnd, 0, NULL);
	int posX = 0;
	int posY = 0;

	while (x <= 10000) {
		TextOut(hdc, posX, posY, std::to_string(x).c_str(), std::to_string(x).size());
		x++;
		Sleep(10);
		QueryPerformanceCounter(&EnInc);
	}
	Sleep(10);
	ReleaseDC(hWnd, hdc);

	return 0;
}

DWORD PrintDecrease(PVOID p) {
	QueryPerformanceCounter(&StDec);

	HWND hWnd = (HWND)p;
	HDC hdc = GetDC(hWnd);
	RECT rt;
	GetClientRect(hWnd, &rt);

	int posX = 0;
	int posY = 20;

	for (; y > 0;) {
		if (y == 9999)
			PostMessage(hWnd, WM_APP_MY_THREAD_UPDATE, 0, (LPARAM)40);
		if (y == 999)
			PostMessage(hWnd, WM_APP_MY_THREAD_UPDATE, 0, (LPARAM)40);
		if (y == 99)
			PostMessage(hWnd, WM_APP_MY_THREAD_UPDATE, 0, (LPARAM)40);
		if (y == 9)
			PostMessage(hWnd, WM_APP_MY_THREAD_UPDATE, 0, (LPARAM)40);

		TextOut(hdc, posX, posY, std::to_string(y).c_str(), std::to_string(y).size());
		y--;
		Sleep(10);
		QueryPerformanceCounter(&EnDec);

	}
	Sleep(10);
	ReleaseDC(hWnd, hdc);
	return 0;
}


DWORD PrintSubtraction(PVOID p) {
	QueryPerformanceCounter(&StSub);

	HWND hWnd = (HWND)p;
	HDC hdc = GetDC(hWnd);
	RECT rt;
	GetClientRect(hWnd, &rt);

	int posX = 0;
	int posY = 40;
	int i = 1;

	while (x - y < 10000) {
		if (x - y == -999)
			PostMessage(hWnd, WM_APP_MY_THREAD_UPDATE, 0, (LPARAM)60);
		if (x - y == -99)
			PostMessage(hWnd, WM_APP_MY_THREAD_UPDATE, 0, (LPARAM)60);
		if (x - y == -9)
			PostMessage(hWnd, WM_APP_MY_THREAD_UPDATE, 0, (LPARAM)60);
		if (x - y == 999)
			PostMessage(hWnd, WM_APP_MY_THREAD_UPDATE, 0, (LPARAM)60);
		if (x - y == 99)
			PostMessage(hWnd, WM_APP_MY_THREAD_UPDATE, 0, (LPARAM)60);
		if (x - y == 9)
			PostMessage(hWnd, WM_APP_MY_THREAD_UPDATE, 0, (LPARAM)60);

		TextOut(hdc, posX, posY, std::to_string(x - y).c_str(), std::to_string(x - y).size());
		i++;
		Sleep(10);
		QueryPerformanceCounter(&EnSub);
	}
	ReleaseDC(hWnd, hdc);
	return 0;
}

void RetreaveTime(PVOID p) {
	HWND hWnd = (HWND)p;
	HDC hdc = GetDC(hWnd);
	RECT rt;

	FILETIME CreationTime;
	FILETIME ExitTime;
	FILETIME KernelTime;
	FILETIME UserTime;
	SYSTEMTIME   lpSystemTime;

	int posX = 40;
	int posY = 40;
	static DWORD exCodeInc, exCodeSub, exCodeDec;
	//	Sleep(20); // задля того
	while (1) {
		char str[40];
		std::string timeIncStr, timeDecStr, timeSubStr;

		double timeInc = (double)(EnInc.QuadPart - StInc.QuadPart) / Fraq.QuadPart;
		sprintf_s(str, "%.2f", timeInc);
		timeIncStr = str;
		double timeDec = (double)(EnDec.QuadPart - StDec.QuadPart) / Fraq.QuadPart;
		memset(str, '\0', sizeof(str));
		sprintf_s(str, "%.2f", timeDec);
		timeDecStr = str;
		double timeSub = (double)(EnSub.QuadPart - StSub.QuadPart) / Fraq.QuadPart;
		memset(str, '\0', sizeof(str));
		sprintf_s(str, "%.2f", timeSub);
		timeSubStr = str;
		TextOut(hdc, 0, posY + 20, "threadInc Time", std::strlen("threadInc Time"));
		TextOut(hdc, posX + 140, posY + 20, timeIncStr.c_str(), timeIncStr.size());
		TextOut(hdc, 0, posY + 40, "threadDec Time", std::strlen("threadInc Time"));
		TextOut(hdc, posX + 140, posY + 40, timeDecStr.c_str(), timeDecStr.size());
		TextOut(hdc, 0, posY + 60, "threadSub Time", std::strlen("threadInc Time"));
		TextOut(hdc, posX + 140, posY + 60, timeSubStr.c_str(), timeSubStr.size());
		Sleep(2);
	}
	ReleaseDC(hWnd, hdc);
}

static void Paint(HWND hWnd, LPPAINTSTRUCT lpPS)
{
	RECT rc;
	HDC hdcMem;
	HBITMAP hbmMem, hbmOld;
	HBRUSH hbrBkGnd;
	HFONT hfntOld;

	//
	// Get the size of the client rectangle.
	//

	GetClientRect(hWnd, &rc);

	//
	// Create a compatible DC.
	//

	hdcMem = CreateCompatibleDC(lpPS->hdc);

	//
	// Create a bitmap big enough for our client rectangle.
	//

	hbmMem = CreateCompatibleBitmap(lpPS->hdc,
		rc.right - rc.left,
		rc.bottom - rc.top);

	//
	// Select the bitmap into the off-screen DC.
	//

	hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

	//
	// Erase the background.
	//

	hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(hdcMem, &rc, hbrBkGnd);
	DeleteObject(hbrBkGnd);

	//
	// Select the font.
	//

	//if (hFont) {
	//hfntOld = (HFONT)SelectObject(hdcMem, hFont);
	//}

	//
	// Render the image into the offscreen DC.
	//

	SetBkMode(hdcMem, TRANSPARENT);
	SetTextColor(hdcMem, GetSysColor(COLOR_WINDOWTEXT));
	//TextOut(hdcMem, 0, 0, szBuf, sizeof(szBuf));


	//if (hfntOld) {
	//	SelectObject(hdcMem, hfntOld);
	//}

	//
	// Blt the changes to the screen DC.
	//

	BitBlt(lpPS->hdc,
		rc.left, rc.top,
		rc.right - rc.left, rc.bottom - rc.top,
		hdcMem,
		0, 0,
		SRCCOPY);

	//
	// Done with off-screen bitmap and DC.
	//

	SelectObject(hdcMem, hbmOld);
	DeleteObject(hbmMem);
	DeleteDC(hdcMem);

}