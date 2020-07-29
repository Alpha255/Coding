#include "Colorful/D3D/D3D11/D3D11Engine.h"
#include "AssetTool/AssetDatabase.h"

D3DTexture::D3DTexture(const std::string& name, const D3D11Device& device)
{
	assert(device.isValid());
	ID3D11Resource* resource = AssetTool::AssetDatabase::instance().tryToLoadD3DTextureFromFile(device.get(), name);
	assert(resource);

	reset(resource);
}
