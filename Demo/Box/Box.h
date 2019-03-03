#pragma once

#include "Util/Geometry.h"
#include "Util/IApplication.h"

class AppBox : public IApplication
{
public:
	AppBox() = default;
	~AppBox() = default;

	virtual void InitScene() override;
	virtual void RenderScene() override;
#if 0
	virtual void UpdateScene(float, float) override;
#endif
protected:
	enum eSpecialEffect
	{
		eNone,
		eInversion,
		eGrayscale,
		eSharpen,
		eBlur,
		eEdgeDetection,
		eEffectCount
	};
private:
	Geometry::Mesh m_BoxMesh;

	RVertexShader m_VertexShader;
	RPixelShader m_PixelShader[eEffectCount];
	RBuffer m_CBufferVS;
	RShaderResourceView m_DiffuseTex;

#if 1
	VulkanContext m_Context;
	VulkanCommandBuffer m_CmdBuffer;
#endif

	int32_t m_Effect = eNone;
};
