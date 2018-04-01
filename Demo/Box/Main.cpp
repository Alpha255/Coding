#include "Box.h"
//
//INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
//{
//	ApplicationBox Box;
//
//	Box.Startup(L"Box");
//
//	Box.Running();
//
//	Box.ShutDown();
//
//	return 0;
//}
#include "Common.h"
#include "D3DEngine.h"
#include "System.h"
#include <iostream>
#include <d3d11.h>

///atexit(fnl);

int32_t WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppBox box;

	box.Startup(L"Box");

	box.Running();

	box.ShutDown();

	return 0;
}