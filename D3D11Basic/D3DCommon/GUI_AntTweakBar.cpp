#include "GUI_AntTweakBar.h"

#include "GUI/AntTweakBar/include/AntTweakBar.h"

GUIAntTweakBar::~GUIAntTweakBar()
{
	TwTerminate();
}

void GUIAntTweakBar::Init(ID3D11Device *pDevice, uint32_t width, uint32_t height, const char *pTitle)
{
	assert(pDevice);

	int res = TwInit(TW_DIRECT3D11, pDevice);
	assert(res);

	TwWindowSize(width, height);

	m_Bar = TwNewBar(pTitle);
	assert(m_Bar);
}

int GUIAntTweakBar::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

void GUIAntTweakBar::Draw()
{
	TwDraw();
}