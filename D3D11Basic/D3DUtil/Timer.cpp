#include "Timer.h"

Timer::Timer()
	: m_SecondsPerCount(0.0)
	, m_DeltaTime(-1.0)
	, m_BaseTime(0)
	, m_PausedTime(0)
	, m_PrevTime(0)
	, m_CurTime(0)
	, m_bStopped(false)
{
	__int64 countsPerSec = 0;
	/// Retrieves the frequency of the performance counter. 
	/// The frequency of the performance counter is fixed at system boot and is consistent across all processors. 
	/// Therefore, the frequency need only be queried upon application initialization, and the result can be cached.
	::QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_SecondsPerCount = 1.0 / (double)countsPerSec;               
}

float Timer::TotalTime() const
{
	/// If we are stopped, do not count the time that has passed since we stopped.
	/// Moreover, if we previously already had a pause, the distance 
	/// mStopTime - mBaseTime includes paused time, which we do not want to count.
	/// To correct this, we can subtract the paused time from mStopTime:  
	///
	///                     |<--paused time-->|
	/// ----*---------------*-----------------*------------*------------*------> time
	///  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime
	if (m_bStopped)
	{
		return (float)(((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
	}

	/// The distance mCurrTime - mBaseTime includes paused time,
	/// which we do not want to count.  To correct this, we can subtract 
	/// the paused time from mCurrTime:  
	///
	///  (mCurrTime - mPausedTime) - mBaseTime 
	///
	///                     |<--paused time-->|
	/// ----*---------------*-----------------*------------*------> time
	///  mBaseTime       mStopTime        startTime     mCurrTime
	else
	{
		return (float)(((m_CurTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
	}
}

float Timer::DeltaTime() const
{
	return (float)m_DeltaTime;
}

void Timer::Reset()
{
	__int64 curTime = 0;
	/// Retrieves the current value of the performance counter, 
	/// which is a high resolution (<1us) time stamp that can be used for time-interval measurements.
	QueryPerformanceCounter((LARGE_INTEGER*)&curTime);

	m_BaseTime = curTime;
	m_PrevTime = curTime;
	m_StopTime = 0;
	m_bStopped = false;
}

void Timer::Start()
{
	__int64	startTime = 0;
	::QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	/// Accumulate the time elapsed between stop and start pairs.
	///
	///                     |<-------d------->|
	/// ----*---------------*-----------------*------------> time
	///  mBaseTime       mStopTime        startTime  

	if (m_bStopped)
	{
		m_PausedTime += (startTime - m_StopTime);

		m_PrevTime = startTime;
		m_StopTime = 0;
		m_bStopped = false;
	}
}

void Timer::Stop()
{
	if (!m_bStopped)
	{
		__int64 curTime = 0;
		::QueryPerformanceCounter((LARGE_INTEGER*)&curTime);

		m_StopTime = curTime;
		m_bStopped = true;
	}
}

void Timer::Tick()
{
	if (m_bStopped)
	{
		m_DeltaTime = 0.0;
		return;
	}

	__int64 curTime = 0;
	::QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
	m_CurTime = curTime;

	m_DeltaTime = (m_CurTime - m_PrevTime) * m_SecondsPerCount;

	m_PrevTime = m_CurTime;

	/// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	/// processor goes into a power save mode or we get shuffled to another
	/// processor, then mDeltaTime can be negative.
	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
}