#include "TaskScheduler.h"

TaskScheduler &TaskScheduler::Instance()
{
	static TaskScheduler s_TaskScheduler;
	return s_TaskScheduler;
}

void TaskScheduler::Initialize(uint32_t taskNum)
{
	for (uint32_t i = 0U; i < taskNum; ++i)
	{
		m_Tasks.emplace_back(std::make_unique<TaskThread>());
	}
}

void TaskScheduler::TaskThread::TryDoTask()
{
	while (true)
	{
		std::function<void()> task = nullptr;
		{
			std::unique_lock<std::mutex> locker(m_Mutex);
			m_Notifier.wait(locker, [this]() {
				return !m_TaskQueue.empty() || m_Stop;
			});

			if (m_Stop)
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

void TaskScheduler::WaitUntilDone()
{
	for (uint32_t i = 0U; i < m_Tasks.size(); ++i)
	{
		m_Tasks[i]->WaitDone();
	}
}

void TaskScheduler::Finalize()
{
	WaitUntilDone();
}