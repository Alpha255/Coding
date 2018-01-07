#include "GameApplication.h"

#include "Timer.h"
#include "Engine.h"

LRESULT GameApplication::MsgProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
	assert(m_pTimer);

	HandleWindowMessage(msg, wParam, lParam);

	Engine::Instance().HandleInput(msg, wParam, lParam);

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void GameApplication::RenderToWindow()
{
	if (!m_bInited)
	{
		Engine::Instance().Init(m_hWnd, m_Width, m_Height);
		m_bInited = true;
	}

	Engine::Instance().Update(m_pTimer->DeltaTime(), m_pTimer->TotalTime());

	Engine::Instance().RenderScene();
}

GameApplication::~GameApplication()
{
	Engine::Destory();
}
