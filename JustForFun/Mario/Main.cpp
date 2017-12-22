#include "GameApplication.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	GameApplication Mario;

	Mario.Startup(L"Mario");

	Mario.Running();

	Mario.ShutDown();

	return 0;
}