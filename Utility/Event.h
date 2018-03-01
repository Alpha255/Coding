#pragma once

#include "Common.h"

class Event
{
public:
	inline Event()
	{
	}

	inline Event(bool bManualReset, bool bInitialState)
	{
		m_Handle = ::CreateEventA(nullptr, bManualReset, bInitialState, nullptr);
		assert(m_Handle);
	}

	inline ~Event()
	{
		::CloseHandle(m_Handle);
	}

	inline void SetDone()
	{
		::SetEvent(m_Handle);
	}

	inline void Reset()
	{
		::ResetEvent(m_Handle);
	}

	inline void WaitUntilDone()
	{
		::WaitForSingleObject(m_Handle, INFINITE);
	}

	inline bool WaitForTimeout(uint32_t time)
	{
		return ::WaitForSingleObject(m_Handle, time) == WAIT_OBJECT_0;
	}

	inline bool TryWait()
	{
		return ::WaitForSingleObject(m_Handle, 0U) == WAIT_OBJECT_0;
	}
protected:
private:
	::HANDLE m_Handle = nullptr;
};
