#pragma once

#include "Gear/Definitions.h"

namespaceStart(Gear)

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
			///verify_Log(::InitializeCriticalSectionEx(&m_CriticalSection, spinCount, CRITICAL_SECTION_NO_DEBUG_INFO) != 0);
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
			///verify_Log(m_Mutex != nullptr);
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
		///verify_Log(::ReleaseMutex(m_Mutex) != 0);
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
	enum ePriority
	{
		eLow,
		eNormal,
		eHight,
		ePriority_MaxEnum
	};

	enum eAffinity
	{
		eCoreAny,
		eCore_0,
	};

	enum eState
	{
		eSuspend,
		eRunning,
		eTerminated,
		eState_MaxEnum
	};

	enum eThreadType
	{
		eUnited,
		eIndependent,
		eThreadType_MaxEnum
	};

	thread(
		const std::string &name, 
		eThreadType type = eUnited,
		ePriority priority = eNormal, 
		eAffinity affinity = eCoreAny, 
		bool8_t suspendOnStart = true)
		: m_Name(name)
		, m_Type(type)
		, m_Priority(priority)
		, m_Affinity(affinity)
		, m_bSuspend(suspendOnStart)
	{
	}
	virtual ~thread() {}

	uintptr_t getNativeHandle()
	{
		return (uintptr_t)(m_Thread.native_handle());
	}

	std::thread::id getID() const
	{
		return m_Thread.get_id();
	}

	inline void run() 
	{

	}

	inline void suspend()
	{

	}

	inline void terminate()
	{

	}

	inline bool8_t waitDone() 
	{
		if (m_Type == eUnited && m_State == eRunning && m_Thread.joinable())
		{
			m_Thread.join();
			return true;
		}

		return false;
	}

	virtual void threadFunc() = 0;
protected:
private:
	std::string m_Name;
	eThreadType m_Type = eUnited;
	ePriority m_Priority = eNormal;
	eAffinity m_Affinity = eCoreAny;
	eState m_State = eSuspend;

	bool8_t m_bTerminate = false;
	bool8_t m_bSuspend = true;

	std::thread m_Thread;
	std::condition_variable m_Notifier;
};

class threadTask
{
public:
	virtual void execute() = 0;
protected:
private:
};

typedef lockGuard<criticalSection> lockGuardCS;
typedef lockGuard<mutex> lockGuardMutex;
typedef lockGuard<event> lockGuardEvent;
typedef lockGuard<semaphore> lockGuardSemaphore;

namespaceEnd(Gear)
