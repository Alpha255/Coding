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

	SimpleMesh() = default;
	~SimpleMesh() = default;

	void CreateFromTxt(const char *pName, ID3DBlob *pBlob);

	void Draw(bool bWireframe = false);
protected:
private:
	uint32_t m_VertexCount = 0U;
	uint32_t m_IndexCount = 0U;

	Ref<ID3D11InputLayout> m_InputLayout;
	Ref<ID3D11Buffer> m_IndexBuffer;
	Ref<ID3D11Buffer> m_VertexBuffer;
	Ref<ID3D11RasterizerState> m_WireframeMode;

	bool m_Created = false;
};