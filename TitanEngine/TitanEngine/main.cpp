#include "stdafx.h"
#include "D3D12DrawTriangle.h"



_Use_decl_annotations_
int WINAPI	WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	D3D12DrawTriangle sample(1280, 720, L"Draw Triangle");
	return Win32Application::Run(&sample, hInstance, nCmdShow);
}

