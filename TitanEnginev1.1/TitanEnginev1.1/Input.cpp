#include "stdafx.h"
#include "Input.h"
#include "Win32Window.h"

POINT Input::mLastMousePos;


void Input::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

//	SetCapture();
}

void Input::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Input::OnMouseMove(WPARAM btnState, int x, int y)
{
	float dx = glm::radians(0.25f * static_cast<float>(x - mLastMousePos.x));
	float dy = glm::radians(0.25f * static_cast<float>(y - mLastMousePos.y));
	// float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
	 //float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

//	camera.RotateY(dx);
//	camera.Pitch(dy);
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
