#pragma once

#include "gear/Public/Util.h"

namespaceStart(gear)

class taskScheduler : public singleton<taskScheduler>
{
	singletonDeclare(taskScheduler);

public:
	void initialize(uint32_t threadCount = UINT32_MAX);
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
	class taskThread
	{
	public:
		inline taskThread()
		{
			m_Thread = std::thread(&taskThread::processTaskQueue, this);
		}

		inline ~taskThread()
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
	std::vector<std::unique_ptr<taskThread>> m_TaskThreads;
	uint32_t m_TaskIndex = 0U;
};

namespaceEnd(gear)