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
	~GameApplication() = default;

	virtual	LRESULT MsgProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);
	virtual void RenderToWindow();
protected:
private:
	bool m_bInited = false;
};


