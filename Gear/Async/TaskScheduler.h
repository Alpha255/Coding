#pragma once

#include "Gear/DesignPatterns/Singleton.h"

NAMESPACE_START(Gear)

class TaskScheduler : public TSingleton<TaskScheduler>
{
	DECLARE_SINGLETON(TaskScheduler);

public:
	void initialize(uint32_t threadCount = std::numeric_limits<uint32_t>().max());
	void finalize();

	inline void appendTask(std::function<void()> taskFunction)
	{
		m_TaskIndex %= m_TaskThreads.size();
		m_TaskThreads[m_TaskIndex]->appendTask(std::move(taskFunction));
		++m_TaskIndex;
	}

	inline void appendTask(uint32_t threadIndex, std::function<void()> taskFunction)
	{
		assert(threadIndex < m_TaskThreads.size());
		m_TaskThreads[threadIndex]->appendTask(std::move(taskFunction));
	}

	void waitDone();
protected:
	class TaskThread
	{
	public:
		inline TaskThread()
		{
			m_Thread = std::thread(&TaskThread::processTaskQueue, this);
		}

		inline ~TaskThread()
		{
			if (m_Thread.joinable())
			{
				waitDone();
				{
					std::lock_guard<std::mutex> locker(m_Mutex);
					m_bStop = true;
					m_Notifier.notify_one();
				}

				m_Thread.join();
			}
		}

		inline void appendTask(std::function<void()> taskFunction)
		{
			std::lock_guard<std::mutex> locker(m_Mutex);
			m_TaskQueue.push(std::move(taskFunction));
			m_Notifier.notify_one();
		}

		inline void waitDone()
		{
			std::unique_lock<std::mutex> locker(m_Mutex);
			m_Notifier.wait(locker, [this]() {
				return m_TaskQueue.empty();
			});
		}
	protected:
		void processTaskQueue();
	private:
		bool8_t m_bStop = false;
		std::thread m_Thread;
		std::queue<std::function<void()>> m_TaskQueue;
		std::mutex m_Mutex;
		std::condition_variable m_Notifier;
	};
private:
	std::vector<std::unique_ptr<TaskThread>> m_TaskThreads;
	uint32_t m_TaskIndex = 0U;
};

NAMESPACE_END(Gear)
