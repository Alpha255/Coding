#pragma once

void imGUI_D3D_Init(void *pHwnd, struct ID3D11Device *pD3DDevice, struct ID3D11Context *pD3DContext);

void imGUI_D3D_Draw();

void imGUI_D3D_Shutdown();
