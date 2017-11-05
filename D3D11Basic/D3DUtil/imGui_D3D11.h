#pragma once

void imGUI_D3D_Init(void *pHwnd);

void imGUI_D3D_Resize(bool bRecreateVB = false, bool bRecreateIB = false, struct ImDrawData *pDrawData = nullptr);

void imGUI_D3D_RenderBegin();

void imGUI_D3D_RenderEnd();

void imGUI_D3D_Shutdown();
