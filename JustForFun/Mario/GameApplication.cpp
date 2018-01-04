#include "GameApplication.h"

#include "Engine.h"

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
		Engine::Instance().LoadResource();
		m_bInited = true;
	}
}
