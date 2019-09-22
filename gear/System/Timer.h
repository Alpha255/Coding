#pragma once

#include "gear/Public/Util.h"

namespaceStart(gear)

class timer
{
public:
	timer();

	float32_t totalTime() const;
	inline float32_t elapsedTime() const
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