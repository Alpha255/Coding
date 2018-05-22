#pragma once

#include "D3DMath.h"
#include "D3DGeometry.h"
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

	static void DebugDisplay(const Vec3 &position, const eLightType type, const class Camera &cam, const float scale = 0.1f);

protected:
	static Geometry::ObjMesh s_PointLightMesh;

	static D3DVertexShader s_VertexShader;
	static D3DPixelShader s_PixelShader;
	static D3DBuffer s_CBufferVS;

	static bool s_Inited;

	static void Initialize();
};

struct PointLight : public Light
{
	Vec4 Ambient = {};
	Vec4 Diffuse = {};
	Vec4 Specular = {};

	Vec3 Position = {};
	float Range = 0.0f;

	Vec4 Attenuation = {};
};

struct PointLight_1 : public Light
{
	Vec3 Position = {};
	float Range = 10.0f;

	Vec4 Ambient = {};
	Vec4 Diffuse = {};

	Vec3 Specular = {};
	float SpecularIntensity = 32.0f;

	Vec4 Attenuation = {};
};

struct DirectionalLight : public Light
{
	Vec4 Ambient = {};
	Vec4 Diffuse = {};
	Vec4 Specular = {};
	Vec4 Direction = {};
};

struct SpotLight : public Light
{
	Vec4 Ambient = {};
	Vec4 Diffuse = {};
	Vec4 Specular = {};

	Vec3 Position = {};
	float Range = 0.0f;

	Vec3 Direction = {};
	float Spot = 0.0f;

	Vec4 Attenuation = {};
};

struct Material
{
	Vec4 VertexColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	Vec4 Ambient = {};
	Vec4 Diffuse = {};
	Vec4 Specular = {};
	Vec4 Reflection = {};
};

struct Material_1
{
	enum eProperty
	{
		eAmbient,
		eDiffuse,
		eSpecular,
		eNormal,
		ePropertyCount
	};

	struct RawMaterial
	{
		//Vec4 Property[ePropertyCount] = {};
		//uint32_t UsingRaw[ePropertyCount] = {};
		Vec4 Ambient = {};

		Vec3 Diffuse = {};
		uint32_t UsingRawDiffuse = 0U; 

		Vec3 Specular = {};
		uint32_t UsingRawSpecular = 0U;

		Vec3 Normal = {};
		uint32_t UsingRawNormal = 0U;
	};

	inline void Set(eProperty target, const Vec4 &value)
	{
		assert(target < ePropertyCount && target != eNormal/* && RawValue.UsingRaw[target] == 0U*/);
		if (eAmbient == target)
		{
			memcpy(&RawValue.Ambient, &value, sizeof(Vec3));
		}
		else if (eDiffuse == target)
		{
			memcpy(&RawValue.Diffuse, &value, sizeof(Vec3));
			RawValue.UsingRawDiffuse = 1U;
		}
		else if (eSpecular == target)
		{
			memcpy(&RawValue.Specular, &value, sizeof(Vec3));
			RawValue.UsingRawSpecular = 1U;
		}
		//RawValue.Property[target] = value;
		//RawValue.UsingRaw[target] = 1U;
	}

	inline void Set(eProperty target, const char *pTextureName)
	{
		assert(target < ePropertyCount && target != eAmbient /*&& RawValue.UsingRaw[target] == 0U*/ && !Textures[target - 1U].IsValid());
		Textures[target - 1U].Create(pTextureName);
	}

	RawMaterial RawValue;
	D3DShaderResourceView Textures[ePropertyCount - 1U];
};
