#pragma once

#include "Common.h"

class GUIAntTweakBar
{
public:
	typedef void(__stdcall *SetVarCallback)(const void *pValue, void *pClientData);
	typedef void(__stdcall *GetVarCallback)(void *pValue, void *pClientData);
	typedef void(__stdcall *ButtonCallback)(void *pClientData);

	enum eWidgeType
	{
		eNone,
		eCheckBox
	};

	struct WidgeDesc
	{
		eWidgeType Type = eNone;
		SetVarCallback SetVarFunc = nullptr;
		GetVarCallback GetVarFunc = nullptr;
		ButtonCallback BtnFunc = nullptr;
		char *Title = nullptr;
		void *BindData = nullptr;
	};

	GUIAntTweakBar() = default;
	~GUIAntTweakBar();

	void Init(struct ID3D11Device *pDevice, uint32_t width, uint32_t height, const char *pTitle = "AntTweakBar");

	int WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void AddWidget(const WidgeDesc &);

	void Draw();

protected:
private:
	struct CTwBar* m_Bar = nullptr;
};
