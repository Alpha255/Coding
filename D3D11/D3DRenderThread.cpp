#include "D3DRenderThread.h"
#include "D3DEngine.h"

void D3DRenderThread::Register()
{
	D3DEngine::Instance().RegisterRenderThread(GetThreadID());
}
