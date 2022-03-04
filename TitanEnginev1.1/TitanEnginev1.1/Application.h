#pragma once

class Application
{
public:
	virtual bool Initialize() = 0;
	virtual int Run() = 0;
};

