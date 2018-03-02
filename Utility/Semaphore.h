#pragma once

#include "Common.h"

class Semaphore
{
public:
	inline Semaphore(uint32_t initialCount, uint32_t maxCount)
	{
		m_Handle = ::CreateSemaphoreExA(nullptr, initialCount, maxCount, nullptr, 0U, SEMAPHORE_ALL_ACCESS);
		assert(m_Handle);
	}

	inline ~Semaphore()
	{
		::CloseHandle(m_Handle);
	}

	inline void Lock()
	{
		::WaitForSingleObject(m_Handle, INFINITE);
	}

	inline void Unlock()
	{
		::ReleaseSemaphore(m_Handle, 1L, nullptr);
	}

	inline bool TryLock()
	{
		return ::WaitForSingleObject(m_Handle, 0U) != WAIT_TIMEOUT;
	}
protected:
private:
	::HANDLE m_Handle = nullptr;
};
