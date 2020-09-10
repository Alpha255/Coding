#pragma once

#include "Gear/Definitions.h"

NAMESPACE_START(Gear)

class IAsyncObject 
{
protected:
	uint64_t Handle = 0u;
};

class CriticalSection : public IAsyncObject
{
public:
	void lock();
	void unlock();
	bool8_t tryLock();
protected:
private:
};

class Mutex
{
};

class Event
{
};

class Semaphore
{
};

class Thread
{
};

NAMESPACE_END(Gear)
