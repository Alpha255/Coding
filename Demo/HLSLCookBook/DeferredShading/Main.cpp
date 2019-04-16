
#include "DeferredShading.h"

int32_t WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppDeferredShading DeferredShading;

	DeferredShading.Startup(L"DeferredShading", 1280U,720U);

	DeferredShading.Running();

	DeferredShading.ShutDown();

	return 0;
}
