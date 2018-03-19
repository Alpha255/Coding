#include "AlphaBlend.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppAlphaBlend alphaBlend;

	alphaBlend.Startup(L"AlphaBlend");

	alphaBlend.Running();

	alphaBlend.ShutDown();

	return 0;
}