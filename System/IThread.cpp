#include "IThread.h"

static DWORD WINAPI ThreadRunCallback(void *pThread)
{
	assert(pThread);

	IThread *pCurThread = static_cast<IThread *>(pThread);

	assert(pCurThread);

	pCurThread->ThreadEntryPoint();

	return 0U;
}

void IThread::Suspend()
{
	assert(::GetCurrentThreadId() != m_ThreadID);

	m_ThreadLock.Lock();
	if (IsRunning() || IsSuspened())
	{
		assert(m_Handle);
		::SuspendThread(m_Handle);
		
		m_StateLock.Lock();
		m_State = eSuspended;
		m_StateLock.Unlock();
	}
	else
	{
		assert(0);
	}
	m_ThreadLock.Unlock();
}

bool IThread::Resume(bool bForceResume)
{
	assert(::GetCurrentThreadId() != m_ThreadID);
	assert(!IsSuspened());

	m_ThreadLock.Lock();
	uint32_t suspendCount = 0U;
	do
	{
		assert(m_Handle);
		suspendCount = ::ResumeThread(m_Handle);
	} while (bForceResume && suspendCount > 1U);

	if (suspendCount <= 1)
	{
		/// It is possible that the thread finished between the ResumeThread() and this point. (Win64)
		/// In this case, we must not set the thread running because it will indicate a false state and lead to possible misbehaviors.
		m_StateLock.Lock();
		if (!IsStopped())
		{
			m_State = eRunning;
		}
		m_StateLock.Unlock();

		return true;
	}

	assert(!bForceResume);
	m_ThreadLock.Unlock();

	return false;
}

bool IThread::InternalStartThread(bool /*bStackSizeChanged*/)
{
	m_ThreadLock.Lock();
	if (!IsStopped())
	{
		assert(0);
	}
	else if (m_Handle)
	{
		Reset();
	}

	m_Handle = ::CreateThread(
		nullptr, 
		m_StackSize, 
		ThreadRunCallback, 
		this, 
		CREATE_SUSPENDED, 
		(LPDWORD)&m_ThreadID
	);

	if (!m_Handle)
	{
		Reset();
		return false;
	}

	m_StateLock.Lock();
	m_State = eSuspended;
	m_StateLock.Unlock();

	InternalSetAffinity();

	InternalSetPriority();

	InternalSetPriorityBoost();

	Resume(true);
	m_ThreadLock.Unlock();

	return true;
}

void IThread::ThreadEntryPoint()
{
	eState localState = eStopped;

	m_StartEvent.SetDone();

	m_StateLock.Lock();
	localState = m_State;
	m_StateLock.Unlock();

	if (localState != eDestroying)
	{
		m_StateLock.Lock();
		m_State = eRunning;
		m_StateLock.Unlock();

		Run();
	}
	else
	{
		assert(0);
	}

	m_StateLock.Lock();
	m_State = eStopped;
	m_StateLock.Unlock();

	m_StopEvent.SetDone();
}