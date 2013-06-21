#pragma once

#include <windows.h>

// Core window handling class.
class Window 
{
private:
	HINSTANCE hInst;
	HWND hWnd;

public:
	Window();
	~Window();

	bool setupWindow(long x, long y, long width, long height);

	bool move(long x, long y); 	
	bool resize(long width, long height);		
	void showMouse(bool bshow = true); 

	HWND getHWnd() const;
	HINSTANCE getHInst() const;
};
//////////////////////////////////////////////////////////////////////////////////
class System 
{
	protected:
		Window			mWindow;

		long			x; // X-Coordinate of the top-left corner of the window.
		long			y; // Y-Coordinate of the top-left corner of the window.
		long			width;
		long			height;
		bool			windowed;

		virtual void onProcess() { }
		virtual void shutdown();

	public:
		System();
		~System() { shutdown(); }
		
		virtual bool onInit();

		void messageLoop();
		Window & getWindow() { return mWindow; }

		void parseCommandLine(int argc, char * argv[]);
		
		virtual LRESULT CALLBACK MsgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, Msg, wParam, lParam); }		
};

extern System gSystem;

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);