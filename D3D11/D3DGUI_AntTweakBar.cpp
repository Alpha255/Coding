#include "GUI_AntTweakBar.h"

#include "GUI/AntTweakBar/include/AntTweakBar.h"
#include "D3DGraphic.h"

extern D3DGraphic *g_Renderer;

GUIAntTweakBar::~GUIAntTweakBar()
{
	TwTerminate();
}

int GUIAntTweakBar::Init(HWND hWnd)
{
	assert(hWnd && g_Renderer);

	int res = TwInit(TW_DIRECT3D11, g_Renderer->GetDevice());
	assert(res);

	::RECT rect;
	::GetClientRect(hWnd, &rect);

	TwWindowSize((int)(rect.right - rect.left), (int)(rect.bottom - rect.top));

	m_Bar = TwNewBar("AntTweakBar");

	return res;
}

int GUIAntTweakBar::WinProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
	return TwEventWin(hWnd, msg, wParam, lParam);
}

void GUIAntTweakBar::AddWidget(const WidgeDesc &desc)
{
	switch (desc.Type)
	{
	case eCheckBox:
		TwAddVarCB(m_Bar, desc.Title, TW_TYPE_BOOLCPP, desc.SetVarFunc, desc.GetVarFunc, nullptr, nullptr/*"group=Sponge key=o"*/);
		break;
	}
}

void GUIAntTweakBar::RenderEnd()
{
	assert(m_Bar);

	TwDraw();
}