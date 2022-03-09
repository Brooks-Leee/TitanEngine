#pragma once

class Window
{
public:
	static Window* Create();

	virtual bool initWindow() = 0;
	virtual bool Run() = 0;
	void Destroy(Window* window);

public:
	int mClientWidth = 800;
	int mClientHeight = 600;
};


