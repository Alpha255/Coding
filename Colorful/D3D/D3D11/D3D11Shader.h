#pragma once

#include "Colorful/Public/RAsset.h"

class d3d11Shader : public d3dObject<ID3D11DeviceChild>, public rShader
{
public:
	d3d11Shader(const class d3d11Device &device, eRShaderUsage usage, const rAsset::rShaderBinary &binary);
};