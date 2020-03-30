#include "rRenderTest.h"

void rRenderTest::postInitialize()
{
	m_VertexShader = g_rDevicePtr->createShader(eVertexShader, "rRenderTest.shader");
	m_FragmentShader = g_rDevicePtr->createShader(eFragmentShader, "rRenderTest.shader");
	m_DiffuseTexture = g_rDevicePtr->createTexture("WoodCrate01.dds");

	/// m_DiffuseTexture.bindSampler();
	/// m_VertexShader.setIntputLayout();
	/// m_FragmentShader.pushUniformBuffer();
	/// m_FragmentShader.pushTexture(m_DiffuseTexture);

	/// GraphicPipline0.create();
	/// GraphicPipline0.setShader(m_VertexShader);
	/// GraphicPipline0.setShader(m_FragmentShader);

	/// RenderPass.create("Opaque");
}

void rRenderTest::renterToWindow()
{
	/// RenderPass.bindPipline(GraphicPipline0);
	/// RenderPass.execute();

	/// RenderPass.bindPipline(GraphicPipline1);
	/// RenderPass.execute();

	/// UI Pass
}

appMainEntry(rRenderTest)
