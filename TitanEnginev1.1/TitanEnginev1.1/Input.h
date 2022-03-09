#pragma once
class Input
{
public:
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
	void OnKeyboardInput();

public:
	static POINT mCurrentMousePos;
	static bool mIsMouseDown;
};

