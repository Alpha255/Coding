#include "Gear/Async/Async.h"

NAMESPACE_START(Gear)

void CriticalSection::lock()
{
}

void CriticalSection::unlock()
{
}

bool8_t CriticalSection::tryLock()
{
	return bool8_t();
}

NAMESPACE_END(Gear)
