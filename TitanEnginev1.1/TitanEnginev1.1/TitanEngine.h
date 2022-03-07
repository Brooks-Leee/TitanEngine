#pragma once



class TitanEngine
{
private:
	static TitanEngine* s_engine;


private:
	TitanEngine() {};
	~TitanEngine() {};
	TitanEngine(const TitanEngine&);

public:
	static TitanEngine* Get() {
		return s_engine;
	}


public:
	bool Init(HINSTANCE hInstance);
	void Loop();
	void Destroy();

};

