#pragma once

#include "D3DMath.h"
#include "D3DGeometry.h"

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

	static void DrawLight(const Vec3 &position, const eLightType type, const class Camera &cam);

protected:
	static Geometry::ObjMesh s_PointLightMesh;
	static Geometry::ObjMesh s_DirLightMesh;
	static Geometry::ObjMesh s_SpotLightMesh;

	static D3DVertexShader VertexShader;
	static D3DPixelShader PixelShader;
	static D3DBuffer CBufferVS;

	static bool Inited;

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
