#pragma once

#include "Gear/System.h"

NAMESPACE_START(Gear)

class CriticalSection
{
public:
	CriticalSection(uint32_t spinCount = 0u)
	{
		::InitializeCriticalSectionEx(&m_CriticalSection, spinCount, 0u);
	}
	~CriticalSection()
	{
		::DeleteCriticalSection(&m_CriticalSection);
	}
	CriticalSection(const CriticalSection&) = delete;
	CriticalSection& operator=(const CriticalSection&) = delete;
	CriticalSection(CriticalSection&&) = delete;

	void lock()
	{
		::EnterCriticalSection(&m_CriticalSection);
	}

	void unlock()
	{
		::LeaveCriticalSection(&m_CriticalSection);
	}

	bool8_t tryLock()
	{
		return ::TryEnterCriticalSection(&m_CriticalSection);
	}
protected:
private:
	::CRITICAL_SECTION m_CriticalSection{};
};

class Mutex
{
};

class Event
{
public:
	Event(bool8_t manualReset, bool8_t signal = false)
	{
		m_Event = ::CreateEventEx(
			nullptr, 
			nullptr, 
			(signal ? CREATE_EVENT_INITIAL_SET : 0u) | (manualReset ? CREATE_EVENT_MANUAL_RESET : 0u), 
			EVENT_MODIFY_STATE);
		VERIFY_SYSTEM(m_Event != nullptr);
	}
	~Event()
	{
		if (m_Event)
		{
			::CloseHandle(m_Event);
		}
	}
	Event(const Event&) = delete;
	Event(Event&&) = delete;
	Event& operator=(const Event&) = delete;

	void signal()
	{
		assert(m_Event);
		VERIFY_SYSTEM(::SetEvent(m_Event) != 0);
	}

	void reset()
	{
		assert(m_Event);
		VERIFY_SYSTEM(::ResetEvent(m_Event) != 0);
	}

	bool8_t wait(uint32_t milliseconds)
	{
		assert(m_Event);
		return ::WaitForSingleObjectEx(m_Event, milliseconds, false) == WAIT_OBJECT_0;
	}
protected:
private:
	::HANDLE m_Event = nullptr;
};

class Semaphore
{
};

class Thread
{
};

NAMESPACE_END(Gear)
