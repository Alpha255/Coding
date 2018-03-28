//#include "Box.h"
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

void fnl() 
{ 
	D3DEngine::Instance();
}

int32_t WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	D3DEngine::Instance();

	D3DEngine::Destory();

	///atexit(fnl);

	return 0;
}