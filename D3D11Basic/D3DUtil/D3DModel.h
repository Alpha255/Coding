#pragma once

#include <DirectXTK/Inc/Effects.h>
#include <DirectXTK/Inc/Model.h>
#include <DirectXTK/Inc/CommonStates.h>

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