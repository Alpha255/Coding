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

	Vec4 GetLightSpecular(Light::eLightType type) const;

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

		PointLight PointLights[3];
	};
private:
	std::array<PointLight, 3U> m_PointLights;
	SpotLight m_SpotLight;
	std::array<CapsuleLight, 3U> m_CapsuleLights;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader[Light::eLightTypeCount];
	D3DBuffer m_CBufferPS;
	ConstantBufferPS m_CBufferPSLighting;
	D3DDepthStencilState m_NoDepthWriteLessStencilMask;

	bool m_Inited = false;
};
