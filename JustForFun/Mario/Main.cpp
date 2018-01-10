#include "GameApplication.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	GameApplication Mario;

	Mario.Startup(L"Mario", 800U, 600U, true, WS_THICKFRAME ^ WS_MAXIMIZEBOX);

	Mario.Running();

	Mario.ShutDown();

	return 0;
}