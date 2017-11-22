#pragma once

#include <Windows.h>

void imGUI_D3D_Init(void *pHwnd);

void imGUI_D3D_Resize(bool bRecreateVB = false, bool bRecreateIB = false, struct ImDrawData *pDrawData = nullptr);

void imGUI_D3D_RenderBegin();

void imGUI_D3D_RenderEnd();

void imGUI_D3D_Shutdown();

LRESULT imGUI_WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
