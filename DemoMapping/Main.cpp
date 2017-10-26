#include "NormalDisplacementMapping.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	ApplicationMapping Mapping;

	Mapping.Startup(L"Normal-Displacement-Mapping");

	Mapping.Running();

	Mapping.ShutDown();

	return 0;
}