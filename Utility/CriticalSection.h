#pragma once

#include "Common.h"

class CriticalSection
{
public:
	enum
	{
		eDefaultSpinCount = 0x1000
	};

	inline explicit CriticalSection(uint32_t spinCount = eDefaultSpinCount)
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

	inline ~CriticalSection()
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
		return ::TryEnterCriticalSection(&m_CS) != 0;
	}
protected:
private:
	::CRITICAL_SECTION m_CS = {};
};
