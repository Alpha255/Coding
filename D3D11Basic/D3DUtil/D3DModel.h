#pragma once

#include <DirectXTK/Inc/Effects.h>
#include <DirectXTK/Inc/Model.h>
#include <DirectXTK/Inc/CommonStates.h>
#include "D3DMath.h"

class D3DModel
{
public:
	D3DModel();
	~D3DModel();

	void CreateFromSDKMesh(const wchar_t* pName);

	void Draw(const class Camera& camera, bool bWireframe = false);
	void DrawCustom();

	const ID3D11Buffer* const GetVertexBuffer(uint32_t index) const;
	const ID3D11Buffer* const GetIndexBuffer(uint32_t index) const;
	uint32_t GetVertexStride(uint32_t index) const;
	DXGI_FORMAT GetIndexFormat(uint32_t index) const;
protected:
private:
	DirectX::EffectFactory* m_EffectFactory;
	DirectX::CommonStates* m_States;

	std::unique_ptr<DirectX::Model> m_Model;
};

class SimpleMesh
{
public:
	struct Vertex
	{
		Vec3 Position;
		Vec3 Normal;
	};

	SimpleMesh()
	{
		m_World.Identity();
	}
	~SimpleMesh() = default;

	void CreateFromTxt(const char *pName);

	inline void SetMaterial(const Lighting::Material &mat)
	{
		memcpy(&m_CBufferPS.Mat, &mat, sizeof(Lighting::Material));
	}

	inline void SetLightCount(uint32_t litCount)
	{
		assert(litCount <= 3U);  /// Not safety
		m_CBufferPS.LightCount = litCount;
	}

	inline void SetLight(uint32_t index, const Lighting::DirectionalLight &lit)
	{
		assert(index < 3U);  /// Not safety
		memcpy(&m_CBufferPS.DirLight[index], &lit, sizeof(Lighting::DirectionalLight));
	}

	inline void SetWorldMatrix(const Matrix& world)
	{
		m_World = world;
	}

	void Draw(const class Camera &cam, bool bWireframe = false);
protected:
	struct ConstantsBufferVS
	{
		Matrix World;
		Matrix WorldInverseTrans;
		Matrix WVP;

		ConstantsBufferVS()
		{
			World.Identity();
			WorldInverseTrans.Identity();
			WVP.Identity();
		}
	};

	struct ConstantsBufferPS
	{
		Vec3 ViewPoint;
		uint32_t LightCount = 0U;

		Lighting::DirectionalLight DirLight[3];

		Lighting::Material Mat;

		ConstantsBufferPS()
		{
			ViewPoint = Vec3(0.0f, 0.0f, 0.0f);
			
			DirLight[0].Ambient = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
			DirLight[0].Diffuse = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
			DirLight[0].Specular = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
			DirLight[0].Direction = Vec4(0.57735f, -0.57735f, 0.57735f, 0.0f);

			DirLight[1].Ambient = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
			DirLight[1].Diffuse = Vec4(0.20f, 0.20f, 0.20f, 1.0f);
			DirLight[1].Specular = Vec4(0.25f, 0.25f, 0.25f, 1.0f);
			DirLight[1].Direction = Vec4(-0.57735f, -0.57735f, 0.57735f, 0.0f);

			DirLight[2].Ambient = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
			DirLight[2].Diffuse = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
			DirLight[2].Specular = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
			DirLight[2].Direction = Vec4(0.0f, -0.707f, -0.707f, 0.0f);

			Mat.Ambient = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
			Mat.Diffuse = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
			Mat.Specular = Vec4(0.8f, 0.8f, 0.8f, 16.0f);
		}
	};

	struct D3DResource
	{
		Ref<ID3D11VertexShader> VertexShader;
		Ref<ID3D11PixelShader> PixelShader;

		Ref<ID3D11InputLayout> InputLayout;
		Ref<ID3D11Buffer> IndexBuffer;
		Ref<ID3D11Buffer> VertexBuffer;
		Ref<ID3D11RasterizerState> WireframeMode;

		Ref<ID3D11Buffer> CBufferVS;
		Ref<ID3D11Buffer> CBufferPS;
	};
private:
	ConstantsBufferVS m_CBufferVS;
	ConstantsBufferPS m_CBufferPS;

	D3DResource m_D3DRes;

	Matrix m_World;

	uint32_t m_VertexCount = 0U;
	uint32_t m_IndexCount = 0U;

	bool m_Created = false;
};