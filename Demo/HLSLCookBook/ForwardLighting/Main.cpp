#include "ForwardLighting.h"

int32_t WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppForwardLighting forwardLighting;

	forwardLighting.Startup(L"ForwardLighting");

	forwardLighting.Running();

	forwardLighting.ShutDown();

	return 0;
}