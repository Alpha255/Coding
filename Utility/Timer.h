#pragma once

#include "Common.h"

class Timer
{
public:
	Timer();

	float TotalTime() const;
	float DeltaTime() const;

	void Reset();
	void Start();
	void Stop();
	void Tick();
protected:
private:
	double  m_SecondsPerCount;
	double  m_DeltaTime;

	__int64 m_BaseTime; 
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurTime;

	bool    m_bStopped;
};