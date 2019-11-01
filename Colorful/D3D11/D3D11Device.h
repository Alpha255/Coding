#pragma once

#include <d3d11.h>
#include "Public/RInterface.h"

class d3d11Device : public d3dObject<ID3D11Device>, public rDevice
{
public:
	void create() override final;
};
typedef std::shared_ptr<d3d11Device> d3d11DevicePtr;
