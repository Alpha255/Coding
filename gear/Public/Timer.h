#pragma once

#include "gear/Public/Util.h"

namespaceStart(gear)

class timer
{
public:
	timer();

	inline float32_t getTotalTime() const
	{
		if (m_bStopped)
		{
			return (float32_t)(((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondsPerTick);
		}
		else
		{
			return (float32_t)(((m_CurTime - m_PausedTime) - m_BaseTime) * m_SecondsPerTick);
		}
	}

	inline float32_t getElapsedTime() const
	{
		return (float32_t)m_ElapsedTime;
	}

	void reset();
	void start();
	void stop();
	void tick();
protected:
private:
	double64_t m_SecondsPerTick = 0.0;
	double64_t m_ElapsedTime = 0.0;

	int64_t m_BaseTime = 0; 
	int64_t m_PausedTime = 0;
	int64_t m_StopTime = 0;
	int64_t m_LastTime = 0;
	int64_t m_CurTime = 0;

	bool8_t m_bStopped = false;
};

namespaceEnd(gear)