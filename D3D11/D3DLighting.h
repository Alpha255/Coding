#pragma once

#include "D3DMath.h"
#include "D3DView.h"

struct Light
{
public:
	enum eLightType
	{
		ePoint,
		eDirectional,
		eSpot,
		eLightTypeCount
	};

	struct ShaderParams
	{
		Vec3 Position;
		uint32_t Type;

		Vec3 Direction;
		float Range;

		Vec3 Attenuation;
		float SpotFactor;

		Vec4 Ambient;
		Vec4 Diffuse;
		Vec4 Specular;

		ShaderParams()
		{
			memset(this, 0, sizeof(ShaderParams));
			Type = eLightTypeCount;
		}
	};

	ShaderParams Params;

	void Draw();
protected:
	void Init();

	bool m_Inited = false;
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

		Vec4 VertexColor;

		Vec4 Ambient;
		Vec4 Diffuse;
		Vec4 Specular;
		Vec4 Reflection;

		ShaderParams()
		{
			memset(this, 0, sizeof(ShaderParams));
			VertexColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	};

	Material() = default;
	///Material(const Material &srcMat)
	///{
	///	memcpy(&Params, &srcMat.Params, sizeof(ShaderParams));
	///	for (uint32_t i = 0U; i < eTexTypeCount; ++i)
	///	{
	///		if (srcMat.Textures[i].Valid())
	///		{
	///			Textures[i] = srcMat.Textures[i];
	///		}
	///	}
	///}

	///void operator=(const Material &srcMat)
	///{
	///	memcpy(&Params, &srcMat.Params, sizeof(ShaderParams));
	///	for (uint32_t i = 0U; i < eTexTypeCount; ++i)
	///	{
	///		if (srcMat.Textures[i].Valid())
	///		{
	///			Textures[i] = srcMat.Textures[i];
	///		}
	///	}
	///	Sampler = srcMat.Sampler;
	///}

	ShaderParams Params;

	D3DShaderResourceView Textures[eTexTypeCount];

	///D3DSamplerState Sampler;

	void SetTexture(eTextureType type, const D3DShaderResourceView &texture);
protected:
};
