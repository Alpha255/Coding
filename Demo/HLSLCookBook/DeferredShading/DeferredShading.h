
#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DGeometryBuffer.h"
#include "LightController.h"

class AppDeferredShading : public D3DApp
{
public:
	AppDeferredShading() = default;
	~AppDeferredShading() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
	virtual void ResizeWindow(uint32_t width, uint32_t height) override;
protected:
	struct ConstantBufferVS
	{
		Matrix World;
		Matrix WVP;
	};

	struct ConstantBufferPS
	{
		Vec4 LightSpecular;
	};
private:
	D3DGeometryBuffer m_GBuffer;
	LightController m_LightController;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;

	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	Geometry::SDKMesh m_Bunny;

	int32_t m_LightingType = Light::eDirectional;
	bool m_bVisualizeGBuffer = true;
};
