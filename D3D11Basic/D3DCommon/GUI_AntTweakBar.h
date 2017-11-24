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

	void Init(HWND hWnd);

	void RenderBegin(const char * = "AntTweakBar"){}
	void RenderEnd();

	int WinProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);

	void AddWidget(const WidgeDesc &desc);

	inline bool IsFocus()
	{
		return false;
	}

protected:
private:
	struct CTwBar* m_Bar = nullptr;
};
