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

class Material_1
{
public:
	struct RawMaterial
	{
		Vec4 Ambient = {};
		Vec4 Diffuse = {};
		Vec4 Specular = {};
	};
protected:
private:
	RawMaterial m_RawMaterial;
	D3DShaderResourceView m_DiffuseMap;
	D3DShaderResourceView m_NormalMap;
	D3DShaderResourceView m_SpecularMap;
};
