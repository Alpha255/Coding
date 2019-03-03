#include "Box.h"

int32_t WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppBox box;

	box.Startup(L"Box");

	box.Running();

	box.ShutDown();

	return 0;
}