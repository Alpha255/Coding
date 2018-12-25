//#pragma once
//
//#include "D3DMath.h"
//#include "Definitions.h"
//#include "Lighting.h"
//#include <DirectXTK/Inc/Effects.h>
//#include <DirectXTK/Inc/Model.h>
//#include <DirectXTK/Inc/CommonStates.h>
//
//class Model
//{
//public:
//	Model();
//	~Model();
//
//	void CreateFromSDKMesh(const wchar_t* pName);
//
//	void Draw(const class Camera& camera, bool bWireframe = false);
//
//	uint32_t GetVertexStride(uint32_t index) const;
//	uint32_t GetIndexFormat(uint32_t index) const;
//protected:
//private:
//	DirectX::EffectFactory* m_EffectFactory;
//	DirectX::CommonStates* m_States;
//
//	std::unique_ptr<DirectX::Model> m_Model;
//};
//
//class SDKMesh
//{
//public:
//	struct Vertex
//	{
//		Vec3 Position;
//		Vec3 Normal;
//	};
//
//	SDKMesh()
//	{
//		m_World.Identity();
//	}
//	~SDKMesh() = default;
//
//	void CreateFromTxt(const char *pName);
//
//	inline void SetMaterial(const Material &mat)
//	{
//		memcpy(&m_CBufferPS.Mat, &mat, sizeof(Material));
//	}
//
//	inline void SetLightCount(uint32_t litCount)
//	{
//		assert(litCount <= 3U);  /// Not safety
//		m_CBufferPS.LightCount = litCount;
//	}
//
//	inline void SetLight(uint32_t index, const DirectionalLight &lit)
//	{
//		assert(index < 3U);  /// Not safety
//		memcpy(&m_CBufferPS.DirLight[index], &lit, sizeof(DirectionalLight));
//	}
//
//	inline void SetWorldMatrix(const Matrix& world)
//	{
//		m_World = world;
//	}
//
//	void Draw(const class Camera &cam, bool bWireframe = false);
//protected:
//	struct ConstantsBufferVS
//	{
//		Matrix World;
//		Matrix WorldInverseTrans;
//		Matrix WVP;
//
//		ConstantsBufferVS()
//		{
//			World.Identity();
//			WorldInverseTrans.Identity();
//			WVP.Identity();
//		}
//	};
//
//	struct ConstantsBufferPS
//	{
//		Vec3 ViewPoint;
//		uint32_t LightCount = 0U;
//
//		DirectionalLight DirLight[3];
//
//		Material Mat;
//
//		ConstantsBufferPS()
//		{
//			ViewPoint = Vec3(0.0f, 0.0f, 0.0f);
//			
//			DirLight[0].Ambient = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
//			DirLight[0].Diffuse = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
//			DirLight[0].Specular = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
//			DirLight[0].Direction = Vec4(0.57735f, -0.57735f, 0.57735f, 0.0f);
//
//			DirLight[1].Ambient = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
//			DirLight[1].Diffuse = Vec4(0.20f, 0.20f, 0.20f, 1.0f);
//			DirLight[1].Specular = Vec4(0.25f, 0.25f, 0.25f, 1.0f);
//			DirLight[1].Direction = Vec4(-0.57735f, -0.57735f, 0.57735f, 0.0f);
//
//			DirLight[2].Ambient = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
//			DirLight[2].Diffuse = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
//			DirLight[2].Specular = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
//			DirLight[2].Direction = Vec4(0.0f, -0.707f, -0.707f, 0.0f);
//
//			Mat.Ambient = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
//			Mat.Diffuse = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
//			Mat.Specular = Vec4(0.8f, 0.8f, 0.8f, 16.0f);
//		}
//	};
//
//	struct D3DObject
//	{
//		Ref<ID3D11VertexShader> VertexShader;
//		Ref<ID3D11PixelShader> PixelShader;
//
//		Ref<ID3D11InputLayout> InputLayout;
//		Ref<ID3D11Buffer> IndexBuffer;
//		Ref<ID3D11Buffer> VertexBuffer;
//		Ref<ID3D11RasterizerState> WireframeMode;
//		Ref<ID3D11RasterizerState> BackFaceCulling;
//
//		Ref<ID3D11Buffer> CBufferVS;
//		Ref<ID3D11Buffer> CBufferPS;
//	};
//private:
//	ConstantsBufferVS m_CBufferVS;
//	ConstantsBufferPS m_CBufferPS;
//
//	D3DObject m_D3DRes;
//
//	Matrix m_World;
//
//	uint32_t m_VertexCount = 0U;
//	uint32_t m_IndexCount = 0U;
//
//	bool m_Created = false;
//};
//
//class ObjMesh
//{
//public:
//	ObjMesh() = default;
//	~ObjMesh() = default;
//
//	void Create(const char *pFileName);
//
//	inline const std::vector<Math::Geometry::BasicVertex> &GetVertices() const
//	{
//		assert(m_Vertices.size());
//		return m_Vertices;
//	}
//
//	inline const std::vector<uint32_t> &GetIndices() const
//	{
//		assert(m_Indices.size());
//		return m_Indices;
//	}
//protected:
//	struct ObjIndex
//	{
//		uint32_t i = 0U;
//		uint32_t t = 0U;
//		uint32_t n = 0U;
//	};
//
//	void ObjMesh::CreateVI(
//		const std::vector<Vec3> &srcVertices,
//		const std::vector<ObjIndex> &objIndices,
//		const std::vector<Vec3> &normals,
//		const std::vector<Vec2> &uvs);
//private:
//	std::vector<Math::Geometry::BasicVertex> m_Vertices;
//	std::vector<uint32_t> m_Indices;
//};