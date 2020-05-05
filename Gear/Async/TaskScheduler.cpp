#include "Gear/Async/TaskScheduler.h"

namespaceStart(Gear)

void TaskScheduler::initialize(uint32_t threadCount)
{
	if (threadCount == std::numeric_limits<uint32_t>().max())
	{
		threadCount = std::thread::hardware_concurrency();
	}

	for (uint32_t i = 0U; i < threadCount; ++i)
	{
		m_TaskThreads.emplace_back(std::make_unique<TaskThread>());
	}
}

void TaskScheduler::TaskThread::processTaskQueue()
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

void TaskScheduler::waitDone()
{
	for (uint32_t i = 0U; i < m_TaskThreads.size(); ++i)
	{
		m_TaskThreads[i]->waitDone();
	}
}

void TaskScheduler::finalize()
{
	waitDone();
}

namespaceEnd(Gear)
