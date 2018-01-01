#include "GameApplication.h"

#include "Image.h"

LRESULT GameApplication::MsgProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
	assert(m_pTimer);

	HandleWindowMessage(msg, wParam, lParam);

	HandleInput(msg, wParam, lParam);

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void GameApplication::RenderToWindow()
{
	if (!m_bInited)
	{
		InitResource();
		m_bInited = true;
	}
}

void GameApplication::InitResource()
{
	Image imageTest;
	imageTest.Create("mario.bmp");
}
