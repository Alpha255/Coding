#pragma once

#include "Colorful/D3D/DxgiObject.h"

class d3d12Context final : public D3DObject<ID3D12DeviceContext>, public rContext
{

};
