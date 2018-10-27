#pragma once

#include "D3DView.h"

NamespaceBegin(D3DUtil)

void CreateShaderResourceViewFromDds(const char *pDdsName, _Out_ ID3D11ShaderResourceView **ppShaderResourceView, bool bSRGB);

NamespaceEnd(D3DUtil)