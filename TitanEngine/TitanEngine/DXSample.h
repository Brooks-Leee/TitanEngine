#pragma once

#include "DXSampleHelper.h"
#include "Win32Application.h"

class DXSample
{
public:
	DXSample(UINT width, UINT height, std::wstring name);
	virtual ~DXSample();

	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnDestroy() = 0;

	// sample override the event handlers to handle specific messages
	virtual void OnKeyDown(UINT8 /*key*/) {}
	virtual void OnKeyUp(UINT8 /*key*/) {}

	// accessors
	UINT GetWidth() const { return m_width; }
	UINT GetHeight() const { return m_height; }	
	const WCHAR* GetTitle() const { return m_title.c_str(); }	


protected:
	std::wstring GetAssetFullPath(LPCWSTR assetName);

	void GetHardwareAdapter(
		_In_ IDXGIFactory1* pFactory,
		_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter = false);

	void SetCustomWindowText(LPCWSTR text);

	// viewport
	UINT m_width;
	UINT m_height;
	float m_aspectRatio;

	// adapter info
	bool m_useWarpDevice;

private:
	// root assets path
	std::wstring m_assetsPath;

	// window title
	std::wstring m_title;




};