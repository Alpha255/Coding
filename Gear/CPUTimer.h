#pragma once

#include "Gear/Definitions.h"

namespaceStart(Gear)

class CpuTimer
{
public:
	CpuTimer()
	{
		reset();
	}

	inline float32_t totalTime() const
	{
		if (m_Stopped)
		{
			return std::chrono::duration_cast<Seconds>(m_StopTimePoint - m_BaseTimePoint).count() - m_PausedTime;
		}
		
		return std::chrono::duration_cast<Seconds>(m_CurrentTimePoint - m_BaseTimePoint).count() - m_PausedTime;
	}

	inline float32_t elapsedTime() const
	{
		return m_ElapsedTime;
	}

	inline void reset()
	{
		m_Stopped = false;
		m_ElapsedTime = 0.0f;

		m_BaseTimePoint = std::chrono::high_resolution_clock::now();
		m_LastTimePoint = std::chrono::high_resolution_clock::now();
		m_StopTimePoint = std::chrono::high_resolution_clock::time_point();
	}

	inline void start()
	{
		if (m_Stopped)
		{
			m_PausedTime += std::chrono::duration_cast<Seconds>(std::chrono::high_resolution_clock::now() - m_StopTimePoint).count();
			m_LastTimePoint = std::chrono::high_resolution_clock::now();
			m_StopTimePoint = std::chrono::high_resolution_clock::time_point();
			m_Stopped = false;
		}
	}
	
	inline void stop()
	{
		if (!m_Stopped)
		{
			m_StopTimePoint = std::chrono::high_resolution_clock::now();
			m_Stopped = true;
		}
	}

	inline void tick()
	{
		if (m_Stopped)
		{
			m_ElapsedTime = 0.0f;
			return;
		}

		m_CurrentTimePoint = std::chrono::high_resolution_clock::now();
		m_ElapsedTime = std::chrono::duration_cast<Seconds>(m_CurrentTimePoint - m_LastTimePoint).count();
		m_LastTimePoint = m_CurrentTimePoint;
		m_ElapsedTime = m_ElapsedTime < 0.0f ? 0.0f : m_ElapsedTime;
	}
protected:
	using Seconds = std::chrono::duration<float32_t, std::ratio<1, 1>>;
	using Milliseconds = std::chrono::duration<float32_t, std::ratio<1, 1000>>;
private:
	std::chrono::high_resolution_clock::time_point m_BaseTimePoint;
	std::chrono::high_resolution_clock::time_point m_StopTimePoint;
	std::chrono::high_resolution_clock::time_point m_LastTimePoint;
	std::chrono::high_resolution_clock::time_point m_CurrentTimePoint;

	float32_t m_ElapsedTime = 0.0f;
	float32_t m_TotalTime = 0.0f;
	float32_t m_PausedTime = 0.0f;

	bool8_t m_Stopped = true;
};

namespaceEnd(Gear)
