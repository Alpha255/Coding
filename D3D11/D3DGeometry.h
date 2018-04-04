#pragma once

#include "D3DLighting.h"
#include "D3DBuffer.h"
#include "D3DShader.h"

class Camera;

NamespaceBegin(Geometry)

struct Vertex
{
	Vec3 Position = {};
	Vec3 Normal = {};
	Vec3 Tangent = {};
	Vec2 UV = {};

	Vertex() = default;

	Vertex(const Vec3 &pos, const Vec3 &normal, const Vec3 &tangent, const Vec2 &uv)
		: Position(pos)
		, Normal(normal)
		, Tangent(tangent)
		, UV(uv)
	{
	}

	Vertex(float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v)
		: Position(px, py, pz)
		, Normal(nx, ny, nz)
		, Tangent(tx, ty, tz)
		, UV(u, v)
	{
	}
};

class Mesh
{
public:
	Mesh() = default;
	~Mesh() = default;

	void CreateAsBox(float width, float height, float depth);
	void CreateAsCube(float width);
	void CreateAsSphere(float radius, uint32_t slice, uint32_t stack);
	void CreateAsGeoSphere(float radius, uint32_t subDivisions);
	void CreateAsFlatGeoSphere(float radius, uint32_t subDivisions);
	void CreateAsCylinder(float bottomRadius, float topRadius, float height, uint32_t slice, uint32_t stack);
	void CreateAsGrid(float width, float depth, uint32_t m, uint32_t n);
	void CreateAsQuad(float length);

	void SetLight(const Light *pLight, bool bReset = false);

	void SetMaterial(const Material *pMaterial, bool bReset = false);

	inline void Rotate(float x, float y, float z, float angle)
	{
		m_Transform.Rotate = Matrix::RotationAxis(x, y, z, angle);
	}

	inline void Translation(float x, float y, float z)
	{
		m_Transform.Translation = Matrix::Translation(x, y, z);
	}

	inline void Scale(float factor)
	{
		m_Transform.Scale = Matrix::Scaling(factor, factor, factor);
	}

	inline void Scale(float x, float y, float z)
	{
		m_Transform.Scale = Matrix::Scaling(x, y, z);
	}

	inline void SetWireframe(bool bWireframe)
	{
		m_Wireframe = bWireframe;
	}

	void Draw(const Camera &cam);

	void DrawNormal(const Camera &cam);
protected:
	struct RenderResource
	{
		D3DInputLayout VertexLayout;

		D3DVertexShader VertexShader;
		D3DPixelShader PixelShader;

		D3DBuffer CBufferVS;
		D3DBuffer CBufferPS;

		D3DBuffer VertexBuffer;
		D3DBuffer IndexBuffer;
	};

	struct ConstantBufferVS
	{
		Matrix World;
		Matrix WorldInverse;
		Matrix WorldViewProj;

		Vec4 EyePos;
	};

	struct ConstantBufferPS
	{
		Light::ShaderParams LightParams;

		Material::ShaderParams MaterialParams;

		uint32_t EnableLight = 0U;
		uint32_t EnableMaterial = 0U;
		uint64_t Padding = 0U;
	};

	struct Transform
	{
		Matrix Rotate;
		Matrix Translation;
		Matrix Scale;

		Transform()
		{
			Rotate.Identity();
			Translation.Identity();
			Scale.Identity();
		}
	};

	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	void Init();
	void ApplyMaterial();
	void ApplyLight();

	void SubDivide();
	void MakeCylinderTopBottomCap(bool bTop, float bottomRadius, float topRadius, float height, uint32_t slice);
private:
	RenderResource m_Resource;

	ConstantBufferVS m_CBufferVS;
	ConstantBufferPS m_CBufferPS;

	Transform m_Transform;

	Material m_Material;

	Light m_Light;

	bool m_LightEnabled = false;
	bool m_MaterialEnabled = false;
	bool m_Created = false;
	bool m_Inited = false;
	bool m_Wireframe = false;
};

class ObjMesh : public Mesh
{
public:
	ObjMesh() = default;
	~ObjMesh() = default;

	void Create(const char *pFileName);
protected:
private:
};

NamespaceEnd(Geometry)
