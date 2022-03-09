#include "stdafx.h"
#include "Window.h"
#include "Win32Window.h"


Window* Window::Create()
{
	Window* window = nullptr;
	#if defined (_PLATFORM_WINDOWS)
	window = new Win32Window;
	#endif

	#if defined (_PLATFORM_IOS)

	#endif
	return window;
	
}

void Window::Destroy(Window* window)
{
	delete window;
}
