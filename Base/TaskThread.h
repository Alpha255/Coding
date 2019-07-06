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

class TaskThread
{
public:
	typedef void(ThreadFunc)(void *);

	TaskThread() = default;
	virtual ~TaskThread();

	template<typename Functor>
	inline void Bind(Functor functor)
	{
		if (nullptr == m_Functor)
		{
			m_Functor = functor;
		}
		else
		{
			assert(0);
		}
	}

	void Start(void *pParams);

	inline void Suspend()
	{
		m_Suspend = true;
	}

	inline void Stop()
	{
		m_Stop = true;
	}
protected:
private:
	std::atomic<bool> m_Stop = false;
	std::atomic<bool> m_Suspend = true;

	std::unique_ptr<std::thread> m_Thread;
	std::function<ThreadFunc> m_Functor;
};