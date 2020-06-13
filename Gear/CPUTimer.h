#pragma once

#include "Gear/Definitions.h"

namespaceStart(Gear)

class CpuTimer
{
public:
	enum eState
	{
		eReady,
		eStarted
	};

	CpuTimer(float32_t speed = 1.0f)
		: m_Speed(speed)
	{
		reset();
	}

	inline void setSpeed(float32_t speed)
	{
		if (m_State == eReady)
		{
			m_Speed = speed;
		}
	}

	inline float32_t elapsedTime() const
	{
		return m_ElapsedTime;
	}

	inline float32_t elapsedTimeInMilliseconds() const
	{
		return m_ElapsedTimeInMS;
	}

	inline void reset()
	{
		m_State = eReady;
		m_StartTimestamp = std::chrono::high_resolution_clock::time_point();
		m_StopTimestamp = std::chrono::high_resolution_clock::time_point();
	}

	inline void start()
	{
		if (m_State == eReady)
		{
			m_StartTimestamp = std::chrono::high_resolution_clock::now();
			m_State = eStarted;
		}
	}
	
	inline void stop()
	{
		if (m_State == eStarted)
		{
			m_StopTimestamp = std::chrono::high_resolution_clock::now();

			m_ElapsedTime = std::chrono::duration_cast<Seconds>(m_StopTimestamp - m_StartTimestamp).count();
			m_ElapsedTimeInMS = std::chrono::duration_cast<Milliseconds>(m_StopTimestamp - m_StartTimestamp).count();

			reset();
		}
	}
protected:
	using Seconds = std::chrono::duration<float32_t, std::ratio<1, 1>>;
	using Milliseconds = std::chrono::duration<float32_t, std::ratio<1, 1000>>;
private:
	std::chrono::high_resolution_clock::time_point m_StartTimestamp;
	std::chrono::high_resolution_clock::time_point m_StopTimestamp;
	float32_t m_Speed = 1.0f;
	float32_t m_ElapsedTime = 0.0f;
	float32_t m_ElapsedTimeInMS = 0.0f;
	eState m_State = eReady;
};

namespaceEnd(Gear)
