#include "CThread.h"
#include <windowsx.h>
#include <tchar.h>
#include <time.h>

#define THREADS_NUMBER    4
#define WINDOWS_NUMBER    10
#define SLEEP_TIME       500

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM uParam, LPARAM lParam);

class Thread :public CThread
{
protected:
	virtual void Run(LPVOID lpParameter = 0);
};

void Thread::Run(LPVOID lpParameter)
{
	// 随机种子生成器
	srand((unsigned)time(NULL));
	HWND hWnd = (HWND)GetUserData();
	RECT rect;

	// 获得窗口 尺寸
	BOOL bError = GetClientRect(hWnd,&rect);
	if (!bError)
	{
		return;
	}
	int iClientX = rect.right - rect.left;
	int iClientY = rect.bottom - rect.top;

	// 如果窗口没有尺寸，不绘图
	if ((!iClientX) || (!iClientY))
	{
		return;
	}

	// 获取设备上下文以绘图
	HDC hDC = GetDC(hWnd);
	if (hDC)
	{
		// 绘制10个随机他也想
		for (int iCount = 0; iCount < WINDOWS_NUMBER; iCount++)
		{
			// 设置坐标
			int iStartX = (int)(rand() % iClientX);
			int iStopX = (int)(rand() % iClientX);
			int iStartY = (int)(rand() % iClientY);
			int iStopY = (int)(rand() % iClientY);

			// 设置颜色
			int iRed = rand() & 255;
			int iGreen = rand() & 255;
			int iBlue = rand() & 255;

			// 创建画刷
			HANDLE hBrush = CreateSolidBrush(GetNearestColor(hDC,RGB(iRed,iGreen,iBlue)));
			HANDLE hbrOld = SelectBrush(hDC,hBrush);

			Rectangle(hDC, min(iStartX, iStopX), max(iStartX, iStopX), min(iStartY, iStopY), max(iStartY, iStopY));

			// 删除画刷
			DeleteBrush(SelectBrush(hDC,hbrOld));
		}

		// 释放设备上下文
		ReleaseDC(hWnd,hDC);
	}
	Sleep(SLEEP_TIME);
	return;
}


int WINAPI _tWinMain(HINSTANCE hThis, HINSTANCE hPrev, LPTSTR szCommandLine, int iCmdShow)
{
	WNDCLASSEX wndEx = { 0 };
	wndEx.cbClsExtra = 0;
	wndEx.cbSize = sizeof(WNDCLASSEX);
	wndEx.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wndEx.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndEx.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wndEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndEx.hInstance = hThis;
	wndEx.lpfnWndProc = WindowProcedure;
	wndEx.lpszClassName = _T("async_thread");
	wndEx.lpszMenuName = NULL;
	wndEx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	if (!RegisterClassEx(&wndEx))
	{
		return 1;
	}

	HWND hWnd = CreateWindow(wndEx.lpszClassName,TEXT("Basic Thread Management"),WS_OVERLAPPEDWINDOW,200,200,840,440,
		HWND_DESKTOP,NULL,wndEx.hInstance,NULL);
	HWND hRects[THREADS_NUMBER];

	hRects[0] = CreateWindow(_T("STATIC"),_T(""),WS_BORDER | WS_CHILD |WS_VISIBLE |WS_CLIPCHILDREN,20,20,180,350,hWnd
	,NULL,hThis,NULL);
	hRects[1] = CreateWindow(_T("STATIC"), _T(""), WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 220, 20, 180, 350, hWnd
		, NULL, hThis, NULL);
	hRects[2] = CreateWindow(_T("STATIC"), _T(""), WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 420, 20, 180, 350, hWnd
		, NULL, hThis, NULL);
	hRects[3] = CreateWindow(_T("STATIC"), _T(""), WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 620, 20, 180, 350, hWnd
		, NULL, hThis, NULL);

	UpdateWindow(hWnd);
	ShowWindow(hWnd,SW_SHOW);

	Thread threads[THREADS_NUMBER];
	for (int iIndex = 0; iIndex < THREADS_NUMBER; iIndex++)
	{
		threads[iIndex].Create(NULL,STATE_ASYNC | STATE_CONTINUOUS);
		threads[iIndex].SetUserData(hRects[iIndex]);
	}

	SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)threads);

	MSG msg = { 0 };
	while (GetMessage(&msg, 0, 0, 0));
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnregisterClass(wndEx.lpszClassName,wndEx.hInstance);
	return 0;

}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	case WM_CLOSE:
	{
		Thread* pThread = (Thread*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
		pThread->Alert();
		for (int iIndex = 0; iIndex < THREADS_NUMBER; iIndex++)
		{
			pThread[iIndex].Join();
		}
		DestroyWindow(hWnd);
		break;
	}
	default:
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	}
	return 0;
}