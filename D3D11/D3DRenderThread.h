#pragma once

#include <thread>

class D3DRenderThread
{
public:
	D3DRenderThread() = default;
	~D3DRenderThread()
	{
		if (m_Thread.joinable())
		{
			m_Thread.join();
		}
	}

	inline std::thread::id GetThreadID() const
	{
		///assert(m_Thread.get_id() != std::thread::id());
		return m_Thread.get_id();
	}

	inline void Start(std::thread &&workerThread)
	{
		m_Thread = std::move(workerThread);

		Register();
	}
protected:
	void Register();
private:
	std::thread m_Thread;
};