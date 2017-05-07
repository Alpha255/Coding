#include "Lighting.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	ApplicationLighting Lighting;

	Lighting.Startup(L"Lighting");

	Lighting.Running();

	Lighting.ShutDown();

	return 0;
}