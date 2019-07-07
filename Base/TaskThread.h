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
	inline bool IsSignaled() const
	{
		return m_Signaled;
	}
protected:
private:
	bool m_Signaled = false;
	std::condition_variable m_Signal;
	std::mutex m_Mutex;
};

template <typename ParamType> class TaskThread
{
public:
	enum eState
	{
		eRunning,
		eSuspend,
		eStopped
	};

	typedef void(ThreadFunc)(const ParamType &);

	TaskThread() = default;
	virtual ~TaskThread()
	{
		Stop();

		if (m_Thread && m_Thread->joinable())
		{
			m_Thread->join();
		}
	}

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

	inline void Start(const ParamType &param)
	{
		assert(m_Functor);

		if (m_Suspend)
		{
			m_State = eRunning;
			m_Suspend = false;
		}

		m_Event.Reset();

		if (!m_Thread)
		{
			m_Thread.reset(new std::thread([this, param]() {

				m_Param = param;

				while (!m_Stop)
				{
					if (m_Suspend)
					{
						std::this_thread::yield();
					}
					else
					{
						if (!m_Event.IsSignaled())
						{
							m_Functor(m_Param);

							m_Event.Signal();
						}
						else
						{
							std::this_thread::yield();
						}
					}
				}
			}));
		}
	}

	inline void Suspend()
	{
		m_State = eSuspend;
		m_Suspend = true;
	}

	inline void Stop()
	{
		m_State = eStopped;
		m_Stop = true;
	}

	inline bool IsDone() const
	{
		return m_Event.IsSignaled();
	}

	inline std::thread::id GetID() const
	{
		assert(m_Thread);
		return m_Thread->get_id();
	}

	inline void UpdateParam(const ParamType &param)
	{
		std::lock_guard<std::mutex> locker(m_Mutex);
		m_Param = param;
	}
protected:
private:
	std::atomic<bool> m_Stop = false;
	std::atomic<bool> m_Suspend = true;
	std::mutex m_Mutex;
	Event m_Event;

	ParamType m_Param;

	std::atomic<eState> m_State = eSuspend;

	std::unique_ptr<std::thread> m_Thread;
	std::function<ThreadFunc> m_Functor;
};