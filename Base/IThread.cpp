#include "IThread.h"

void Event::Reset()
{
	std::unique_lock<std::mutex> locker(m_Mutex);
	m_Signaled = false;
}

void Event::Wait(uint32_t microsecond)
{
	std::unique_lock<std::mutex> locker(m_Mutex);

	if (microsecond == 0U)
	{
		m_Signal.wait(locker, [this]()
		{
			return m_Signaled;
		});
	}
	else
	{
		auto waitTime = std::chrono::system_clock::now() + std::chrono::duration<uint32_t, std::milli>(microsecond);
		m_Signal.wait_until(locker, waitTime, [this]()
		{
			return m_Signaled;
		});
	}

	m_Signaled = false;
}

void Event::Signal()
{
	std::unique_lock<std::mutex> locker(m_Mutex);
	m_Signaled = true;
	m_Signal.notify_all();
}

IThread::IThread()
{
	m_Thread = std::thread([=]()
	{
		while (!m_Stop)
		{
			if (m_Suspend)
			{
				m_Signal.Wait(100U);
			}
			else
			{
				ThreadFunc();
			}
		}
	});
}

IThread::~IThread()
{
	Stop();
	m_Signal.Signal();

	if (m_Thread.joinable())
	{
		m_Thread.join();
	}
}
