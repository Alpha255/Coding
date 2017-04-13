#include "Box.h"

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	ApplicationBox Box(hInstance, L"Box");

	return Box.MainLoop();
}