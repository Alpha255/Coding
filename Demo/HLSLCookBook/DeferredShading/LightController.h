#pragma once

#include "D3DLighting.h"
#include "D3DShader.h"
#include "D3DState.h"
#include "D3DBuffer.h"

class LightController
{
public:
	LightController() = default;
	~LightController() = default;

	void Init();

	inline void TurnonTheLights(Light::eLightType lightType, const class D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix)
	{
		assert(m_Inited && lightType < Light::eLightTypeCount);
		
		switch (lightType)
		{
		case Light::ePoint: PointLighting(GBuffer, camPerspective, camViewMatrix); break;
		case Light::eDirectional: DirectionalLighting(GBuffer, camPerspective, camViewMatrix); break;
		case Light::eSpot: SpotLighting(GBuffer, camPerspective, camViewMatrix); break;
		case Light::eCapsule: CapsuleLighting(GBuffer, camPerspective, camViewMatrix); break;
		}
	}

	inline const DirectionalLight &GetDirectionalLight() const
	{
		assert(m_Inited);
		return m_DirLight;
	}
protected:
	void PointLighting(const class D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix);
	void DirectionalLighting(const class D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix);
	void SpotLighting(const class D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix);
	void CapsuleLighting(const class D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix);

	struct ConstantBufferPS
	{
		Vec4 AmbientLowerClr;
		Vec4 AmbientRange;

		Vec4 PerspectiveValue;

		Matrix ViewMatirxInvese;

		DirectionalLight DirLight;
	};
private:
	std::array<PointLight, 3U> m_PointLights;
	DirectionalLight m_DirLight;
	SpotLight m_SpotLight;
	std::array<CapsuleLight, 3U> m_CapsuleLights;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;
	D3DBuffer m_CBufferPS;
	D3DDepthStencilState m_NoDepthWriteLessStencilMask;

	Vec4 m_AmbientLowerClr = { 0.1f, 0.2f, 0.1f, 1.0f };
	Vec4 m_AmbientUpperClr = { 0.1f, 0.2f, 0.2f, 1.0f };

	bool m_Inited = false;
};
