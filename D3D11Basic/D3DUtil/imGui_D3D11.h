#pragma once

void imGUI_D3D_Init(void *pHwnd);

void imGUI_D3D_ResizeVertexIndexBuffer(bool bRecreateVB = false, bool bRecreateIB = false, struct ImDrawData *pDrawData = nullptr);

void imGUI_D3D_Draw(float elapsedTime, float totalTime);

void imGUI_D3D_Shutdown();
