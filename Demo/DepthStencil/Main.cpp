#include "Stenciling.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	ApplicationStenciling stenciling;

	stenciling.Startup(L"Stenciling");

	stenciling.Running();

	stenciling.ShutDown();

	return 0;
}