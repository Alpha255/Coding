#include "D3DLighting.h"

void Light::Init()
{
	if (m_Inited)
	{
		return;
	}

	m_Inited = true;
}

void Light::Draw()
{
	Init();
}

void Material::SetTexture(eTextureType type, const D3DShaderResourceView &texture)
{
	assert(type < eTexTypeCount);

	Textures[type] = texture;

	Params.EnableTexture[type] = 1U;
}