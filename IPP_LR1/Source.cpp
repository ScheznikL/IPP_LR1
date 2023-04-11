#include <windows.h> 
#include <string> 
const UINT WM_APP_MY_THREAD_UPDATE = WM_APP + 0;


HINSTANCE hInst; 	
LPCTSTR szWindowClass = "var2";
LPCTSTR szTitle = "IPP_Lab1";
int x = 1;
int y = 10000;
LARGE_INTEGER Fraq;
LARGE_INTEGER StInc, EnInc, StDec, EnDec, StSub, EnSub;

DWORD PrintIncrease(PVOID p);
DWORD PrintDecrease(PVOID p);
DWORD PrintSubtraction(PVOID p);
void RetreaveTime(PVOID p);

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
	int nCmdShow)
{
	MSG msg;

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

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
	wcex.style = CS_HREDRAW | CS_VREDRAW; 		
	wcex.lpfnWndProc = (WNDPROC)WndProc; 		
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance; 			
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); 		
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); 	
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW); 
	wcex.lpszMenuName = NULL; 				
	wcex.lpszClassName = szWindowClass; 		
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex); 			
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; 
	hWnd = CreateWindow(szWindowClass, 	
		szTitle, 				
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZE,		
		CW_USEDEFAULT, 			
		CW_USEDEFAULT,			
		500, 		
		200, 			
		NULL, 					
		NULL, 					
		hInstance, 				
		NULL); 				

	if (!hWnd) 
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow); 	
	UpdateWindow(hWnd); 			
	return TRUE;
}

HANDLE threadInc, threadDec, threadSub, timeThread;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt;
	DWORD id;
	switch (message)
	{
	case WM_CREATE: {		
		if (QueryPerformanceFrequency(&Fraq) == 0) {
			MessageBox(hWnd, "Your hardware doens't support a high-resolution performance counter!\nTime values will be unclear.",
				"Warning", MB_ICONWARNING);
		}

		threadInc = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PrintIncrease, hWnd, 0, NULL);
		threadDec = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PrintDecrease, hWnd, 0, NULL);
		threadSub = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PrintSubtraction, hWnd, 0, NULL);
		timeThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RetreaveTime, hWnd, 0, NULL);
		break;
	}

	case WM_PAINT: 				
		hdc = BeginPaint(hWnd, &ps); 	
		EndPaint(hWnd, &ps); 	
		break;
	case WM_APP_MY_THREAD_UPDATE: {
		GetClientRect(hWnd, &rt);
		rt.left = 0;
		rt.top = lParam - 20; 

		rt.bottom = lParam;
		InvalidateRect(hWnd, &rt, TRUE);
		break;
	}

	case WM_DESTROY: 				
		PostQuitMessage(0);
		break;
	default:
		
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

DWORD PrintIncrease(PVOID p) {
	QueryPerformanceCounter(&StInc);
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