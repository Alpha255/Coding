#pragma once

#include "gear/Public/Util.h"

namespaceStart(gear)

typedef ::HANDLE threadID;

template <typename T> class lockGuard
{
public:
	inline lockGuard(T &locker)
		: m_Locker(locker)
	{
		m_Locker.lock();
	}

	inline ~lockGuard()
	{
		m_Locker.unlock();
	}
protected:
private:
	T &m_Locker;
};

class criticalSection
{
public:
	inline criticalSection(uint32_t spinCount = 0u, bool8_t bNoDebug = false)
	{
		if (bNoDebug)
		{
			verifyWin(::InitializeCriticalSectionEx(&m_CriticalSection, spinCount, CRITICAL_SECTION_NO_DEBUG_INFO) != 0);
		}
		else if (spinCount > 0u)
		{
			::InitializeCriticalSectionAndSpinCount(&m_CriticalSection, spinCount);
		}
		else
		{
			::InitializeCriticalSection(&m_CriticalSection);
		}
	}

	inline void lock()
	{
		::EnterCriticalSection(&m_CriticalSection);
	}

	inline bool8_t tryLock()
	{
		return ::TryEnterCriticalSection(&m_CriticalSection);
	}

	inline void unlock()
	{
		::LeaveCriticalSection(&m_CriticalSection);
		///::LeaveCriticalSectionWhenCallbackReturns();
	}

	inline threadID getOwner() const
	{
		return m_CriticalSection.OwningThread;
	}

	inline bool8_t isOwnByCurrentThread() const
	{
		return m_CriticalSection.OwningThread == ::GetCurrentThread();
	}

	inline ~criticalSection()
	{
		::DeleteCriticalSection(&m_CriticalSection);
	}
protected:
private:
	::CRITICAL_SECTION m_CriticalSection = {};
};

class mutex
{
public:
	inline mutex(bool8_t initOwner = false, const char *pName = nullptr)
		: m_Name(pName)
	{
		m_Mutex = ::CreateMutexA(nullptr, initOwner, pName);
		if (!m_Mutex && ::GetLastError() == ERROR_ALREADY_EXISTS)
		{
			m_Mutex = ::OpenMutexA(SYNCHRONIZE, false, pName);
			verifyWin(m_Mutex != nullptr);
		}
	}

	inline const std::string &getName() const
	{
		return m_Name;
	}

	inline bool8_t tryLock(uint32_t waitTime = INFINITY)
	{
		return ::WaitForSingleObject(m_Mutex, waitTime) == WAIT_OBJECT_0;
	}

	inline void lock()
	{
		verify(tryLock());
	}

	inline void unlock()
	{
		verifyWin(::ReleaseMutex(m_Mutex) != 0);
	}

	inline ~mutex()
	{
		::CloseHandle(m_Mutex);
	}
protected:
private:
	::HANDLE m_Mutex = nullptr;
	std::string m_Name;
};

class event
{
public:
protected:
private:
};

class semaphore
{
public:
protected:
private:
};

class thread
{
public:
protected:
private:
};

typedef lockGuard<criticalSection> lockGuardCS;
typedef lockGuard<mutex> lockGuardMutex;
typedef lockGuard<event> lockGuardEvent;
typedef lockGuard<semaphore> lockGuardSemaphore;

namespaceEnd(gear)
