#include "LightingBasic.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppLightingBasic lighting;

	lighting.Startup(L"LightingBasic");

	lighting.Running();

	lighting.ShutDown();

	return 0;
}