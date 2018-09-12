
#include "MultithreadedRendering.h"

int32_t WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppMultithreadedRendering MultithreadedRendering;

	MultithreadedRendering.Startup(L"MultithreadedRendering", 1280U, 720U);

	MultithreadedRendering.Running();

	MultithreadedRendering.ShutDown();

	return 0;
}
