#include "Colorful/D3D/D3D11/D3D11Engine.h"

D3DTexture::D3DTexture(const std::string& name, const D3D11Device& device)
{
	assert(device.isValid());
}
