#pragma once

#include "Colorful/D3D/DxgiObject.h"

class d3d11Context final : public d3dObject<ID3D11DeviceContext>, public rContext
{

};
