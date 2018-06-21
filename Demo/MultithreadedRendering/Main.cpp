
#include "MultithreadedRendering.h"

int32_t WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppMultithreadedRendering MultithreadedRendering;

	MultithreadedRendering.Startup(L"MultithreadedRendering");

	MultithreadedRendering.Running();

	MultithreadedRendering.ShutDown();

	return 0;
}
