#include "TaskScheduler.h"

namespaceStart(gear)

void taskScheduler::initialize(uint32_t threadCount)
{
	if (threadCount == UINT32_MAX)
	{
		threadCount = std::thread::hardware_concurrency();
	}

	for (uint32_t i = 0U; i < threadCount; ++i)
	{
		m_TaskThreads.emplace_back(std::make_unique<taskThread>());
	}
}

void taskScheduler::taskThread::processTaskQueue()
{
	while (true)
	{
		std::function<void()> task = nullptr;
		{
			std::unique_lock<std::mutex> locker(m_Mutex);
			m_Notifier.wait(locker, [this]() {
				return !m_TaskQueue.empty() || m_bStop;
			});

			if (m_bStop)
			{
				break;
			}

			task = m_TaskQueue.front();
		}

		task();

		{
			std::lock_guard<std::mutex> locker(m_Mutex);
			m_TaskQueue.pop();
			m_Notifier.notify_one();
		}
	}
}

void taskScheduler::waitDone()
{
	for (uint32_t i = 0U; i < m_TaskThreads.size(); ++i)
	{
		m_TaskThreads[i]->waitDone();
	}
}

void taskScheduler::finalize()
{
	waitDone();
}

namespaceEnd(gear)