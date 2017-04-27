#include "FXAA.h"

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	ApplicationFXAA FXAA(hInstance, L"FXAA");

	return FXAA.MainLoop();
}