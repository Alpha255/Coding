#include "Box.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	ApplicationBox Box;

	Box.Startup(L"Box");

	Box.Running();

	Box.ShutDown();

	return 0;
}