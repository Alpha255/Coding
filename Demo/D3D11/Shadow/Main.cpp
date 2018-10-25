#include "Shadow.h"

int32_t WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppShadow shadow;

	shadow.Startup(L"Shadow");

	shadow.Running();

	shadow.ShutDown();

	return 0;
}