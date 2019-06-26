#pragma once

#include "Base.h"

class Timer
{
public:
	Timer();

	float GetTotalTime() const;
	inline float GetElapsedTime() const
	{
		return (float)m_ElapsedTime;
	}

	void Reset();
	void Start();
	void Stop();
	void Tick();
protected:
private:
	double m_SecondsPerTick = 0.0;
	double m_ElapsedTime = 0.0;

	int64_t m_BaseTime = 0; 
	int64_t m_PausedTime = 0;
	int64_t m_StopTime = 0;
	int64_t m_LastTime = 0;
	int64_t m_CurTime = 0;

	bool m_bStopped = false;
};