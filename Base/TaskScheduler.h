#pragma once

#include "Base.h"

class TaskScheduler
{
public:
	static TaskScheduler &Instance();
	
	void Initialize(uint32_t taskNum);

	inline void AddTask(std::function<void()> func)
	{
		m_TaskIndex %= m_Tasks.size();
		m_Tasks[m_TaskIndex]->AddTask(func);
		++m_TaskIndex;
	}

	void WaitUntilDone();
protected:
	class TaskThread
	{
	public:
		inline TaskThread()
		{
			m_Thread = std::thread(&TaskThread::TryDoTask, this);
		}

		inline ~TaskThread()
		{
			if (m_Thread.joinable())
			{
				WaitDone();
				{
					std::lock_guard<std::mutex> locker(m_Mutex);
					m_Stop = true;
					m_Notifier.notify_one();
				}

				m_Thread.join();
			}
		}

		inline void AddTask(std::function<void()> func)
		{
			std::lock_guard<std::mutex> locker(m_Mutex);
			m_TaskQueue.push(std::move(func));
			m_Notifier.notify_one();
		}

		inline void WaitDone()
		{
			std::unique_lock<std::mutex> locker(m_Mutex);
			m_Notifier.wait(locker, [this]() {
				return m_TaskQueue.empty();
			});
		}

		void TryDoTask();
	protected:
	private:
		bool m_Stop = false;
		std::thread m_Thread;
		std::queue<std::function<void()>> m_TaskQueue;
		std::mutex m_Mutex;
		std::condition_variable m_Notifier;
	};

	TaskScheduler() = default;
	inline ~TaskScheduler()
	{
		Finalize();
	}

	void Finalize();
private:
	std::vector<std::unique_ptr<TaskThread>> m_Tasks;
	uint32_t m_TaskIndex = 0U;
};