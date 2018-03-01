#pragma once

#include "Common.h"

class AdaptiveLock
{
public:
	enum
	{
		eDefaultSpinCount = 0x1000
	};

	inline explicit AdaptiveLock(uint32_t spinCount = eDefaultSpinCount)
	{
		if (spinCount > 0U)
		{
			::InitializeCriticalSectionAndSpinCount(&m_CS, spinCount);
		}
		else
		{
			::InitializeCriticalSection(&m_CS);
		}
	}

	inline ~AdaptiveLock()
	{
		::DeleteCriticalSection(&m_CS);
	}

	inline void Lock()
	{
		::EnterCriticalSection(&m_CS);
	}

	inline void Unlock()
	{
		::LeaveCriticalSection(&m_CS);
	}

	inline bool TryLock()
	{
		return ::TryEnterCriticalSection(&m_CS);
	}
protected:
private:
	::CRITICAL_SECTION m_CS = {};
};
