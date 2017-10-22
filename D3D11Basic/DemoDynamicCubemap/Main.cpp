#include "Cubemap.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	ApplicationCubemap Cubemap;

	Cubemap.Startup(L"Cubemap");

	Cubemap.Running();

	Cubemap.ShutDown();

	return 0;
}