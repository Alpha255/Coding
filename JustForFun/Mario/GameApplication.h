#pragma once

#include "IApplication.h"
#include "resource.h"

class GameApplication : public IApplication
{
public:
	GameApplication()
	{
		m_IconID = IDI_ICON_MARIO;
	}
	~GameApplication();

	virtual	LRESULT MsgProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);
	virtual void RenderToWindow();
	virtual void ResizeWindow(uint32_t width, uint32_t height);
protected:
private:
	bool m_bInited = false;
};


