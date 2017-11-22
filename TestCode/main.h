#pragma once

#include <imgui.h>
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

void CreateRenderTarget();

void CleanupRenderTarget();

HRESULT CreateDeviceD3D(HWND hWnd);

void CleanupDeviceD3D();

void ClearRenderTarget(float *pClr);

void Flip();

void Resize(UINT width, UINT height);

ID3D11Device *GetDevice();

ID3D11DeviceContext *GetContext();