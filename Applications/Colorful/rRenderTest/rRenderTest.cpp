#include "rRenderTest.h"

void rRenderTest::postInitialize()
{
	m_VertexShader = g_rDevicePtr->createShader(eVertexShader, "rRenderTest.shader");
	m_FragmentShader = g_rDevicePtr->createShader(eFragmentShader, "rRenderTest.shader");
	m_DiffuseTexture = g_rDevicePtr->createTexture("WoodCrate01.dds");
}

void rRenderTest::renterToWindow()
{

}

appMainEntry(rRenderTest)
