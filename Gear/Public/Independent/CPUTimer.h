#pragma once

#include "Gear/Public/Definitions.h"

namespaceStart(gear)

class cpuTimer
{
public:
	cpuTimer()
	{
		m_Base = std::chrono::high_resolution_clock::now();
	}

	inline float32_t getTotalTime() const
	{
		std::chrono::high_resolution_clock::time_point totalTime;
		if (m_bStopped)
		{
		}
		else
		{
		}

		return 0.0f;
	}

	inline float32_t getElapsedTime() const
	{
		return m_ElapsedTime;
	}

	inline void reset()
	{
		m_bStopped = false;
		m_ElapsedTime = 0.0f;
	}

	inline void start()
	{
		if (m_bStopped)
		{
			m_Last = std::chrono::high_resolution_clock::now();
			m_Paused += (m_Last - m_Stop);
			m_bStopped = false;
			m_Stop = std::chrono::high_resolution_clock::time_point();
		}
	}
	
	inline void stop()
	{
		if (!m_bStopped)
		{
			m_Stop = std::chrono::high_resolution_clock::now();
			m_bStopped = true;
		}
	}

	inline void tick()
	{
		if (m_bStopped)
		{
			m_ElapsedTime = 0.0f;
			return;
		}

		m_Current = std::chrono::high_resolution_clock::now();
		m_ElapsedTime = std::chrono::duration_cast<std::chrono::duration<float32_t, std::ratio<1, 1000>>>(m_Current - m_Last).count();
		m_Last = m_Current;
		m_ElapsedTime = m_ElapsedTime < 0.0f ? 0.0f : m_ElapsedTime;
	}
protected:
private:
	std::chrono::high_resolution_clock::time_point m_Base;
	std::chrono::high_resolution_clock::time_point m_Stop;
	std::chrono::high_resolution_clock::time_point m_Current;
	std::chrono::high_resolution_clock::time_point m_Last;
	std::chrono::high_resolution_clock::time_point m_Paused;

	float32_t m_ElapsedTime = 0.0f;
	float32_t m_TotalTime = 0.0f;

	bool8_t m_bStopped = false;
};

namespaceEnd(gear)