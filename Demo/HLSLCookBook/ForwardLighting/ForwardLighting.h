#pragma once

#include "Public/IRenderApplication.h"
#include "Colorful/Public/Definitions.h"

class ForwardLighting : public IRenderApplication
{
public:
	ForwardLighting() = default;
	~ForwardLighting() = default;

	void PrepareScene() override;
	void RenderScene() override;
protected:
	inline Vec4 GammaToLinear(const Vec4 &color)
	{
		/// Approximation
		return color * color;
	}
private:
	enum eLightingType
	{
		eHemisphericAmbient,
		eDirectional,
		ePoint,
		eSpot,
		eCapsule,
		eCount
	};

	Geometry::Model m_Bunny;

	RVertexShader m_VertexShader;
	RPixelShader m_PixelShader[eCount];
	RBuffer m_CBufferVS;
	RBuffer m_CBufferPS;
	RInputLayout m_Layout;

#if 1
	RVertexShader m_TestVS;
	RPixelShader m_TestPS;
	RInputLayout m_TestLayout;
	RBuffer m_TestCBVS;
	RBuffer m_TestVB;
	RBuffer m_TestIB;
#endif

	///Material m_BunnyMaterial;

	int32_t m_LightingType = eHemisphericAmbient;
	bool m_Wireframe = false;
};
