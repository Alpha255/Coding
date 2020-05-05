#pragma once

#include "Gear/Definitions.h"

namespaceStart(Gear)

class CpuTimer
{
public:
	inline float32_t totalTime() const
	{
		std::chrono::high_resolution_clock::time_point totalTime;
		if (m_Stopped)
		{
		}
		else
		{
		}

		return 0.0f;
	}

	inline float32_t elapsedTimeInSeconds() const
	{
		return m_ElapsedTime / 1000.0f;
	}

	inline float32_t elapsedTime() const
	{
		return m_ElapsedTime;
	}

	inline void reset()
	{
		m_Stopped = false;
		m_ElapsedTime = 0.0f;
	}

	inline void start()
	{
		if (m_Stopped)
		{
			m_Last = std::chrono::high_resolution_clock::now();
			m_Stopped = false;
			m_Stop = std::chrono::high_resolution_clock::time_point();
		}
	}
	
	inline void stop()
	{
		if (!m_Stopped)
		{
			m_Stop = std::chrono::high_resolution_clock::now();
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

		auto current = std::chrono::high_resolution_clock::now();
		///m_ElapsedTime = std::chrono::duration<float32_t>(current - m_Last).count();
		m_ElapsedTime = std::chrono::duration_cast<std::chrono::duration<float32_t, std::ratio<1, 1000>>>(current - m_Last).count();
		///m_Last = m_Current;
		///m_ElapsedTime = m_ElapsedTime < 0.0f ? 0.0f : m_ElapsedTime;
	}
protected:
private:
	std::chrono::high_resolution_clock::time_point m_Stop;
	std::chrono::high_resolution_clock::time_point m_Last;

	float32_t m_ElapsedTime = 0.0f;
	float32_t m_TotalTime = 0.0f;

	bool8_t m_Stopped = true;
};

namespaceEnd(Gear)
