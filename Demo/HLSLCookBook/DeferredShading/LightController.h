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

	inline void TurnonTheLights(Light::eLightType lightType)
	{
		assert(lightType < Light::eLightTypeCount);
		
		switch (lightType)
		{
		case Light::ePoint: PointLighting(); break;
		case Light::eDirectional: DirectionalLighting(); break;
		case Light::eSpot: SpotLighting(); break;
		case Light::eCapsule: CapsuleLighting(); break;
		}
	}
protected:
	void PointLighting();
	void DirectionalLighting();
	void SpotLighting();
	void CapsuleLighting();
private:
	std::array<PointLight, 3U> m_PointLights;
	DirectionalLight m_DirLight;
	SpotLight m_SpotLight;
	std::array<CapsuleLight, 3U> m_CapsuleLights;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;
	D3DBuffer m_ConstantBufVS;
	D3DDepthStencilState m_NoDepthWriteLessStencilMask;

	Vec3 m_AmbientLowerClr;
	Vec3 m_AmbientUpperClr;
};
