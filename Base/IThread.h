#pragma once

#include "Base.h"

class Event
{
public:
	Event() = default;
	~Event() = default;

	void Reset();
	void Wait(uint32_t microsecond);
	void Signal();
protected:
private:
	bool m_Signaled = false;
	std::condition_variable m_Signal;
	std::mutex m_Mutex;
};

class IThread
{
public:
	IThread();
	virtual ~IThread();

	inline void Start()
	{
		m_Suspend = false;
	}

	inline void Suspend()
	{
		m_Suspend = true;
	}

	inline void Stop()
	{
		m_Stop = true;
	}

	virtual void ThreadFunc() = 0;
protected:
private:
	std::atomic<bool> m_Stop = false;
	std::atomic<bool> m_Suspend = true;

	Event m_Signal;
	std::thread m_Thread;
};