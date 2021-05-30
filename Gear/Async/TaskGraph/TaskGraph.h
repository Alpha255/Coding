#pragma once

#include "Gear/Singleton.h"

NAMESPACE_START(Gear)

class AsyncCounter
{
public:
	inline uint32_t increment()
	{
		return ::_InterlockedIncrement(&m_Count);
	}

	inline uint32_t decrement()
	{
		return ::_InterlockedDecrement(&m_Count);
	}

	inline uint32_t add(uint32_t count)
	{
		return ::_InterlockedExchangeAdd(&m_Count, count);
	}

	inline uint32_t subtract(uint32_t count)
	{
		return ::_InterlockedExchangeAdd(&m_Count, count * -1);
	}

	inline uint32_t set(uint32_t value)
	{
		return ::_InterlockedExchange(&m_Count, value);
	}

	inline uint32_t reset()
	{
		return ::_InterlockedExchange(&m_Count, 0u);
	}

	inline uint32_t count() const
	{
		return ::_InterlockedCompareExchange(const_cast<volatile uint32_t*>(&m_Count), 0u, 0u);
	}
protected:
private:
	volatile uint32_t m_Count = 0;
	void operator=(const AsyncCounter&) {}
};

class Runable
{
public:
	enum EMode
	{
		Multithread,
		SingleThread
	};

	virtual bool8_t init() { return true; }
	virtual uint32_t run() = 0;
	virtual void stop() {}
	virtual void exit() {}
	virtual void tick() {}
	virtual ~Runable() {}
};

class TaskThread
{
public:
	enum EThread
	{
		Main,
		Rendering,
		Audio,
		Task
	};

	enum EPriority
	{
		Idle,
		Lowest,
		BelowNormal,
		Normal,
		AboveNormal,
		Highest,
		TimeCritical

		//Lowest,
		//Low,
		//Normal,
		//High,
		//Highest,
	};

	enum EAffinity
	{
	};
};

class TaskGraph : public TSingleton<TaskGraph>
{
	DECLARE_SINGLETON(TaskGraph);

public:
	void initialize();
	void finalize();
protected:
private:
};

NAMESPACE_END(Gear)