#pragma once

#include <thread>

class D3DRenderThread
{
public:
	D3DRenderThread() = default;
	~D3DRenderThread() = default;

	inline void Start(std::thread &&workerThread)
	{
		m_ThreadID = workerThread.get_id();

		Register();
	}
protected:
	void Register();
private:
	std::thread::id m_ThreadID;
};