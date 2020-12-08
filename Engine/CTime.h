#pragma once

#pragma comment(lib, "Winmm.lib")

class CTime
{
public:
	static CTime& GetInstance() { static CTime instance; return instance; }
	CTime(CTime const&) = delete;
	void operator=(CTime const&) = delete;

	int Init();
	void Update();
	void Release();

	inline double getDeltaTime() { return m_deltaTime; }
	inline double getTime() { return m_time; }

private:
	CTime() {}

	double m_deltaTime;
	double m_lastTimeStamp;
	double m_time;
	int m_fps;
};
