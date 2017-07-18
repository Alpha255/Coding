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
protected:
private:
	DirectX::EffectFactory* m_EffectFactory;
	DirectX::CommonStates* m_States;

	std::unique_ptr<DirectX::Model> m_Model;
};