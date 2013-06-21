#include "System.h"
#include "GraphicsDevice.h"
#include "Define.h"

static System gSystem;
//////////////////////////////////////////////////////////////////////////////////
Window::Window()
{
	hInst = GetModuleHandle(NULL);
}
//////////////////////////////////////////////////////////////////////////////////
Window::~Window()
{
	UnregisterClass("EgoApp", hInst);
}
//////////////////////////////////////////////////////////////////////////////////
bool Window::setupWindow(long x, long y, long width, long height)
{
	WNDCLASSEX wcex;
	
	//create standard extended window	
	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.style         = CS_CLASSDC;
	wcex.lpfnWndProc   = WindowProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInst;
	wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION); // standard icon
	wcex.hCursor       = NULL;
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // black background
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = "EgoApp";
	wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION); // standard icon

	//register class
	if(!RegisterClassEx(&wcex)) 
	{
		OutputDebugString("Couldn't Register Class");
		return false;
	}

	//Create window, using set variables.
	hWnd = CreateWindow("EgoApp", "Ego App", WS_OVERLAPPEDWINDOW, x, y, 
						width, height, NULL, NULL, hInst, NULL
						);

	if(!hWnd)
	{
		OutputDebugString("Couldn't Create Window");
		return false;
	}

	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
bool Window::move(long x, long y) 
{
	RECT ClientRect;

	GetClientRect(hWnd, &ClientRect);
	MoveWindow(hWnd, x, y, ClientRect.right, ClientRect.bottom, TRUE);

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
bool Window::resize(long width, long height) 
{
	RECT WndRect, ClientRect;
	long WndWidth, WndHeight;

	GetWindowRect(hWnd, &WndRect);
	GetClientRect(hWnd, &ClientRect);

	// Find new width, using the window rectangle and client rectangle.
	WndWidth  = (WndRect.right  - (ClientRect.right  - width))  - WndRect.left;

	// Find new height, using the window rectangle and client rectangle.
	WndHeight = (WndRect.bottom - (ClientRect.bottom - height)) - WndRect.top;

	// Move Window can also resize the window.
	MoveWindow(hWnd, WndRect.left, WndRect.top, WndWidth, WndHeight, TRUE);

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
void Window::showMouse(bool Show) 
{
	ShowCursor(Show);
}
//////////////////////////////////////////////////////////////////////////////////
HWND Window::getHWnd() const { return hWnd; }
//////////////////////////////////////////////////////////////////////////////////
HINSTANCE Window::getHInst() const { return hInst; }
//////////////////////////////////////////////////////////////////////////////////
System::System() : x(200), y(200), width(640), height(480), windowed(false) { }
//////////////////////////////////////////////////////////////////////////////////
void System::parseCommandLine(int argc, char * argv[])
{
	for(int i = 0; i < argc; i++)
	{
		if(argv[i][0] != '-')
			continue;

		switch(argv[i][1])
		{
			case 'w':
				windowed = true;
				break;
			case 'm':
				x = 640;
				y = 480;
				break;
			case 'h':
				x = 1024;
				y = 768;
				break;
			case 'u':
				x = 1280;
				y = 1024;
				break;
			default:
				break; // Not recognized.  Do nothing.
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
bool System::onInit()
{
	if(!mWindow.setupWindow(x, y, width, height))
		return false;

	if(!gGraphics.setGraphics(mWindow.getHWnd(), windowed, x, y))
		return false;

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
void System::shutdown()
{
	gGraphics.shutdown();
}
//////////////////////////////////////////////////////////////////////////////////
void System::messageLoop() 
{			
	// While the message is not a destroy window message.
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while(msg.message != WM_QUIT) 
	{
		// Handle any Windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{
			onProcess();
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch(uMsg) 
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		default: return gSystem.MsgProc(hWnd, uMsg, wParam, lParam);
	}
}