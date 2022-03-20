#include "stdafx.h"
#include "Input.h"
#include "Win32Window.h"
#include "TitanEngine.h"


POINT Input::mCurrentMousePos;
bool Input::mIsMouseDown;

void Input::OnMouseDown(WPARAM btnState, int x, int y)
{
	mCurrentMousePos.x = x;
	mCurrentMousePos.y = y;
	mIsMouseDown = true;
//	TitanEngine::Get()->GetSceneIns()->SpawnSphere();
}

void Input::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
	mIsMouseDown = false;

}

void Input::OnMouseMove(WPARAM btnState, int x, int y)
{
	float dx = glm::radians(0.25f * static_cast<float>(x - mCurrentMousePos.x));
	float dy = glm::radians(0.25f * static_cast<float>(y - mCurrentMousePos.y));
	mCurrentMousePos.x = x;
	mCurrentMousePos.y = y;
}
//
//void Input::OnKeyboardInput()
//{
//
//	float dt = 3;
//	if (GetAsyncKeyState('W') & 0x8000)
//		camera.Walk(10.0f * dt);
//
//	if (GetAsyncKeyState('S') & 0x8000)
//		mCamera.Walk(-10.0f * dt);
//
//	if (GetAsyncKeyState('A') & 0x8000)
//		mCamera.Strafe(-10.0f * dt);
//
//	if (GetAsyncKeyState('D') & 0x8000)
//		mCamera.Strafe(10.0f * dt);
//
//}
//

