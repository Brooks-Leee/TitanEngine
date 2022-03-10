#pragma once
#include "Window.h"
#include "Input.h"

class Win32Window : public Window
{
public:

	virtual bool initWindow();
	virtual void InitInput();
	HWND GetHWnd();
	virtual bool Run();
	LRESULT MsgProc(HWND hwd, UINT msg, WPARAM wParam, LPARAM lParam);
	void CalculateFrameStat();

protected:
	HWND HWnd;
	HINSTANCE mhAppInst = nullptr; // application instance handle
	bool      mAppPaused = false;  // is the application paused?
	bool      mMinimized = false;  // is the application minimized?
	bool      mMaximized = false;  // is the application maximized?
	bool      mResizing = false;   // are the resize bars being dragged?
	bool      mFullscreenState = false;// fullscreen enabled
	static Win32Window* mWin;

public:
	Input* mInput;
};

