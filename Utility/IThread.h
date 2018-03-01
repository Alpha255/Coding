#pragma once

#include "AdaptiveLock.h"
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
	}

	virtual void Suspend() = 0;
	virtual bool Resume(bool bForceResume) = 0;
	virtual void Run() = 0;
	virtual bool InternalStart(bool bStackSizeChanged) = 0;

	inline void SetName(const char *pName)
	{
		assert(pName);
		strcpy_s(m_ThreadName, pName);
	}

	inline const char *GetName() const
	{
		return m_ThreadName;
	}
protected:
private:
	eState m_State = eStopped;

	int32_t m_Priority = eNormal;
	int32_t m_Affinity = eNoAffinity;

	uint32_t m_StackSize = 128U * 1024U; /// 128KB

	Event m_StartEvent;
	Event m_StopEvent;

	AdaptiveLock m_StateLock;
	AdaptiveLock m_ThreadLock;

	char m_ThreadName[MAX_PATH] = {};

	bool m_PriorityBoost = true;
	bool m_ThreadStarted = false;
};
