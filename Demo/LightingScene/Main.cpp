#include "LightingScene.h"

int32_t WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppLightingScene lightingScene;

	lightingScene.Startup(L"LightingScene", 1280U, 720U);

	lightingScene.Running();

	lightingScene.ShutDown();

	return 0;
}