#pragma once

#include "CriticalSection.h"
#include "Event.h"

class IThread
{
public:
	enum eState
	{
		eStopped,
		eStarting,
		eRunning,
		eSuspended,
		eDestroying
	};

	enum eAffinity
	{
		eNoAffinity = -1
	};

	enum ePriority
	{
		eIdle = -15,
		eLowest = -2,
		eBelowNormal = -1,
		eNormal = 0,
		eAboveNormal = 1,
		eHighest = 2,
		eTimeCritical = 15
	};

	IThread(const IThread &) = delete;
	void operator =(const IThread &) = delete;

	IThread()
		: m_StartEvent(false, false)
		, m_StopEvent(false, false)
	{
	}

	virtual ~IThread()
	{
		WaitThreadCompletion();
		Reset();
	}

	virtual void Run() = 0;

	void Suspend();
	bool Resume(bool bForceResume);
	void ThreadEntryPoint();
	///void Join() {}

	inline bool BlockUntilStarted(uint32_t time = INFINITE)
	{
		return m_StartEvent.WaitForTimeout(time);
	}

	inline bool BlockUntilStopped(uint32_t time = INFINITE)
	{
		//bool res = true;

		//if (IsThreadValid())
		//{
		//	res = m_StopEvent.WaitForTimeout(time);
		//	if (res)
		//	{
		//		Join();

		//		while (IsThreadValid())
		//		{
		//			::SwitchToThread();
		//		}
		//	}
		//}
		//else
		//{
		//	Join();
		//}

		//return res;
		if (m_Handle)
		{
			uint32_t res = ::WaitForSingleObject(m_Handle, time);
			return res != WAIT_TIMEOUT;
		}

		return false;
	}

	inline bool IsThreadValid() const 
	{
		::HANDLE handle = ::OpenThread(THREAD_ALL_ACCESS, false, m_ThreadID);
		if (nullptr == handle || INVALID_HANDLE_VALUE == handle)
		{
			return false;
		}

		uint32_t res = ::WaitForSingleObject(handle, 0U);
		::CloseHandle(handle);

		return (res != 0U && res != 0xFFFFFFFF);
	}

	inline void SetName(const char *pName)
	{
		assert(pName);
		strcpy_s(m_ThreadName, pName);
	}

	inline const char *GetName() const
	{
		return m_ThreadName;
	}

	inline bool IsStarting() const
	{
		return m_State == eStarting;
	}

	inline bool IsRunning() const
	{
		return m_State == eRunning;
	}

	inline bool IsSuspened() const
	{
		return m_State == eSuspended;
	}

	inline bool IsStopped() const
	{
		return m_State == eStopped;
	}

	inline int32_t GetPriority() const 
	{
		return m_Priority;
	}

	inline void SetPriority(int32_t priority)
	{
		m_Priority = priority;

		if (!IsStopped())
		{
			InternalSetPriority();
		}
	}

	inline int32_t GetAffinity() const
	{
		return m_Affinity;
	}

	inline int32_t GetAffinityMask() const
	{
		::SYSTEM_INFO si = {};
		::GetSystemInfo(&si);
		return ~(0xFFFFFFFF << si.dwNumberOfProcessors);
	}

	inline void SetAffinity(int32_t affinityMask)
	{
		if (affinityMask & GetAffinityMask())
		{
			m_Affinity = affinityMask;

			if (!IsStopped())
			{
				InternalSetAffinity();
			}
		}
	}

	inline void SetPriorityBoost(bool bBoost)
	{
		m_PriorityBoost = bBoost;

		if (!IsStopped())
		{
			InternalSetPriorityBoost();
		}
	}

	inline uint32_t GetStackSize() const
	{
		return m_StackSize;
	}

	inline bool StartThread()
	{
		m_ThreadStarted = true;
		return InternalStartThread(false);
	}

	inline bool StartThread(int32_t priority, int32_t affinity, uint32_t stackSize)
	{
		m_ThreadStarted = true;
		m_Priority = priority;
		m_Affinity = affinity;

		bool bStackSizeChanged = (stackSize > 0) && (m_StackSize != stackSize);
		m_StackSize = bStackSizeChanged ? stackSize : m_StackSize;

		return InternalStartThread(bStackSizeChanged);
	}

	inline void WaitThreadCompletion()
	{
		if (true == m_ThreadStarted)
		{
			m_StateLock.Lock();
			if (!IsStopped())
			{
				m_State = eDestroying;
			}
			m_StateLock.Unlock();

			BlockUntilStopped(INFINITE);
		}
	}
protected:
	bool InternalStartThread(bool bStackSizeChanged);

	inline void InternalSetPriority()
	{
		if (m_Handle)
		{
			int32_t res = ::SetThreadPriority(m_Handle, m_Priority);
			if (0 == res)
			{
				assert(0);
			}
		}
	}

	inline void InternalSetAffinity()
	{
		if (m_Handle && (m_Affinity != eNoAffinity))
		{
			uint64_t res = ::SetThreadAffinityMask(m_Handle, m_Affinity);
			if (res == 0U)
			{
				assert(0);
			}
		}
	}

	inline void InternalSetPriorityBoost()
	{
		if (m_Handle)
		{
			int32_t res = ::SetThreadPriorityBoost(m_Handle, !m_PriorityBoost);
			if (0 == res)
			{
				assert(0);
			}
		}
	}

	inline void Reset()
	{
		assert(IsStopped());

		if (m_Handle)
		{
			int32_t res = ::CloseHandle(m_Handle);
			if (0 == res)
			{
				assert(res);
			}
		}

		m_State = eStopped;
		m_Handle = nullptr;
	}
private:
	::HANDLE m_Handle = nullptr;

	eState m_State = eStopped;

	int32_t m_Priority = eNormal;
	int32_t m_Affinity = eNoAffinity;

	uint32_t m_StackSize = 128U * 1024U; /// 128KB
	uint32_t m_ThreadID = 0U;

	Event m_StartEvent;
	Event m_StopEvent;

	CriticalSection m_StateLock;
	CriticalSection m_ThreadLock;

	char m_ThreadName[_MAX_FNAME] = {};

	bool m_PriorityBoost = true;
	bool m_ThreadStarted = false;
};
