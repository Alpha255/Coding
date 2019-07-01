#include "RenderThread.h"

void RenderThread::ThreadFunc()
{
	static uint32_t index = 0U;
	Base::Log("Thread Test: %d.", index++);

	::Sleep(1000);
}