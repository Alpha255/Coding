#include "IRenderThread.h"

void D3DRenderThread::Register()
{
	D3DEngine::Instance().RegisterRenderThread(GetThreadID());
}
