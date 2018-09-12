#include "GammaCorrection.h"

int32_t WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppGammaCorrection gammaCorrection;

	gammaCorrection.Startup(L"GammaCorrection", 1280U, 720U);

	gammaCorrection.Running();

	gammaCorrection.ShutDown();

	return 0;
}