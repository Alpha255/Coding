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
		eCapsule,
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
	Vec3 Position = { 0.0f, 0.0f, -2.0f };
	float Range = 10.0f;

	Vec4 Ambient = { 0.05f, 0.05f, 0.05f, 1.0f };
	Vec4 Diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };

	Vec3 Specular = { 1.0f, 1.0f, 1.0f };
	float SpecularIntensity = 32.0f;

	Vec4 Attenuation = { 1.0f, 0.045f, 0.0075f, 0.0f };
};

struct DirectionalLight : public Light
{
	Vec4 Direction = {};
	Vec4 Ambient = { 0.05f, 0.05f, 0.05f, 1.0f };
	Vec4 Diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	
	Vec3 Specular = { 1.0f, 1.0f, 1.0f };
	float SpecularIntensity = 32.0f;
};

struct SpotLight : public Light
{
	Vec3 Position = {};
	float Range = 60.0f;

	Vec3 LookAt = {};
	float SpotCosOuterCone = 65.0f;

	Vec3 Attenuation = { 1.0f, 0.045f, 0.0075f };
	float SpotCosInnerCone = 55.0f;

	Vec4 Ambient = { 0.05f, 0.05f, 0.05f, 1.0f };
	Vec4 Diffuse = {};

	Vec3 Specular = { 1.0f, 1.0f, 1.0f };
	float SpecularIntensity = 250.0f;
};

struct CapsuleLight : public Light
{
	Vec3 Position = {};
	float Range = 20.0f;

	Vec3 Direction = {};
	float Length = 85.0f;

	Vec4 Ambient = { 0.05f, 0.05f, 0.05f, 1.0f };
	Vec4 Diffuse = {};

	Vec3 Specular = { 1.0f, 1.0f, 1.0f };
	float SpecularIntensity = 250.0f;
};

struct Material
{
	enum eProperty
	{
		eDiffuse,
		eSpecular,
		eNormal,
		eReflection,
		ePropertyCount
	};

	struct RawMaterial
	{
		Vec4 Porperties[ePropertyCount] = {};
	};

	inline void Set(eProperty target, const Vec4 &value)
	{
		assert(target < ePropertyCount);
		RawValue.Porperties[target] = value;
	}

	inline void Set(eProperty target, const char *pTextureName, bool bFromCurDir = false)
	{
		assert(target < ePropertyCount);
		Textures[target].Create(pTextureName, bFromCurDir);
	}

	RawMaterial RawValue;
	D3DShaderResourceView Textures[ePropertyCount];
};
