#include "D3DLighting.h"

void Light::DrawLight(const Light &light)
{

}

void Material::SetTexture(eTextureType type, const D3DShaderResourceView &texture)
{
	assert(type < eTexTypeCount);

	Textures[type] = texture;

	Params.EnableTexture[type] = 1U;
}