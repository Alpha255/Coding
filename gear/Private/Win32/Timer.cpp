#include "Gear/Public/Extension/Timer.h"

#if defined(Platform_Win32)

namespaceStart(gear)

cpuTimer::cpuTimer()
{
	::LARGE_INTEGER largeInt = {};
	::QueryPerformanceFrequency(&largeInt);
	m_SecondsPerTick = 1.0 / (double)largeInt.QuadPart;
}

void cpuTimer::reset()
{
	::LARGE_INTEGER largeInt = {};
	QueryPerformanceCounter(&largeInt);

	m_BaseTime = largeInt.QuadPart;
	m_LastTime = largeInt.QuadPart;
	m_StopTime = 0;
	m_bStopped = false;
}

void cpuTimer::start()
{
	::LARGE_INTEGER largeInt = {};
	::QueryPerformanceCounter(&largeInt);

	if (m_bStopped)
	{
		m_PausedTime += (largeInt.QuadPart - m_StopTime);

		m_LastTime = largeInt.QuadPart;
		m_StopTime = 0;
		m_bStopped = false;
	}
}

void cpuTimer::stop()
{
	if (!m_bStopped)
	{
		::LARGE_INTEGER largeInt = {};
		::QueryPerformanceCounter(&largeInt);

		m_StopTime = largeInt.QuadPart;
		m_bStopped = true;
	}
}

void cpuTimer::tick()
{
	if (m_bStopped)
	{
		m_ElapsedTime = 0.0;
		return;
	}

	::LARGE_INTEGER largeInt = {};
	::QueryPerformanceCounter(&largeInt);
	m_CurTime = largeInt.QuadPart;

	m_ElapsedTime = (m_CurTime - m_LastTime) * m_SecondsPerTick;

	m_LastTime = m_CurTime;

	if (m_ElapsedTime < 0.0)
	{
		m_ElapsedTime = 0.0;
	}
}

namespaceEnd(gear)

#endif
