#pragma once

#include "D3DGraphic.h"
///#include "D3DTexture.h"

NamespaceBegin(Geometry)

struct Vertex
{
	Vec3 Position;
	Vec3 Normal;
	Vec3 Tangent;
	Vec2 UV;

	Vertex() 
	{ 
		memset(this, 0, sizeof(Vertex)); 
	}

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

	void operator=(const Material &srcMat)
	{
		memcpy(&Params, &srcMat.Params, sizeof(ShaderParams));
		for (uint32_t i = 0U; i < eTexTypeCount; ++i)
		{
			if (srcMat.Textures[i].Valid())
			{
				Textures[i] = srcMat.Textures[i];
			}
		}

		Sampler = srcMat.Sampler;
	}

	ShaderParams Params;

	Ref<ID3D11ShaderResourceView> Textures[eTexTypeCount];

	Ref<ID3D11SamplerState> Sampler;

	void SetTexture(eTextureType type, const Ref<ID3D11ShaderResourceView> &srcTex);
protected:
	void Init();
};

class Mesh
{
public:
	Mesh() = default;
	~Mesh() = default;

	///Mesh(Mesh &&srcMesh);
	///Mesh &operator=(Mesh &&srcMesh);

	static void MakeBox(float width, float height, float depth, Mesh &mesh);
	static void MakeCube(float width, Mesh &mesh);
	static void MakeSphere(float radius, uint32_t slice, uint32_t stack, Mesh &mesh);
	static void MakeGeoSphere(float radius, uint32_t subDivisions, Mesh &mesh);
	static void MakeFlatGeoSphere(float radius, uint32_t subDivisions, Mesh &mesh);
	static void MakeCylinder(float bottomRadius, float topRadius, float height, uint32_t slice, uint32_t stack, Mesh &mesh);
	static void MakeGrid(float width, float depth, uint32_t m, uint32_t n, Mesh &mesh);
	static void MakeQuad(float length, Mesh &mesh);

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

	inline void SetWireframeMode(bool bWireframe)
	{
		m_Wireframe = bWireframe;
	}

	void Draw(const Camera &cam);

	void DrawNormal(const Camera &cam);
	void Init();
protected:
	struct RenderResource
	{
		Ref<ID3D11InputLayout> VertexLayout;

		Ref<ID3D11VertexShader> VertexShader;
		Ref<ID3D11PixelShader> PixelShader;

		Ref<ID3D11Buffer> CBufferVS;
		Ref<ID3D11Buffer> CBufferPS;

		Ref<ID3D11Buffer> VertexBuffer;
		Ref<ID3D11Buffer> IndexBuffer;

		Ref<ID3D11RasterizerState> Wireframe;
	};

	struct ConstantsBufferVS
	{
		Matrix World;
		Matrix WorldInverse;
		Matrix WorldViewProj;

		Vec4 EyePos;
	};

	struct ConstantsBufferPS
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

	void ApplyMaterial();
	void ApplyLight();

	static void SubDivide(Mesh& mesh);
	static void MakeCylinderTopBottomCap(bool bTop, float bottomRadius, float topRadius, float height, uint32_t slice, Mesh &mesh);
private:
	RenderResource m_Resource;

	ConstantsBufferVS m_CBufferVS;
	ConstantsBufferPS m_CBufferPS;

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
