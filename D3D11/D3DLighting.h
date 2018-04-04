#pragma once

#include "D3DMath.h"
#include "D3DView.h"

struct Light
{
	enum eLightType
	{
		ePoint,
		eDirectional,
		eSpot,
		eLightTypeCount
	};

	struct ShaderParams
	{
		Vec3 Position = {};
		uint32_t Type = eLightTypeCount;

		Vec3 Direction = {};
		float Range = 0.0f;

		Vec3 Attenuation = {};
		float SpotFactor = 0.0f;

		Vec4 Ambient = {};
		Vec4 Diffuse = {};
		Vec4 Specular = {};
	};

	ShaderParams Params;

	static void DrawLight(const Light &light);
};

struct Material
{
public:
	enum eTextureType
	{
		eDiffuseMap,
		eNormalMap,
		eSprcularMap,
		eHeightMap,
		eTexTypeCount
	};

	struct ShaderParams
	{
		uint32_t EnableTexture[eTexTypeCount] = {};

		Vec4 VertexColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		Vec4 Ambient = {};
		Vec4 Diffuse = {};
		Vec4 Specular = {};
		Vec4 Reflection = {};
	};

	Material() = default;

	Material(const Material &other)
	{
		memcpy(&Params, &other.Params, sizeof(ShaderParams));
		for (uint32_t i = 0U; i < eTexTypeCount; ++i)
		{
			if (other.Textures[i].IsValid())
			{
				Textures[i] = other.Textures[i];
			}
		}
	}

	void operator=(const Material &other)
	{
		memcpy(&Params, &other.Params, sizeof(ShaderParams));
		for (uint32_t i = 0U; i < eTexTypeCount; ++i)
		{
			if (other.Textures[i].IsValid())
			{
				Textures[i] = other.Textures[i];
			}
		}
	}

	ShaderParams Params;

	D3DShaderResourceView Textures[eTexTypeCount];

	void SetTexture(eTextureType type, const D3DShaderResourceView &texture);
protected:
};
