#include "rRenderTest.h"

void rRenderTest::postInitialize()
{
	m_VertexShader = g_rDevicePtr->createShader(eVertexShader, "rRenderTest.xml");
	m_FragmentShader = g_rDevicePtr->createShader(eFragmentShader, "rRenderTest.xml");
}

void rRenderTest::resizeWindow()
{

}

void rRenderTest::renterToWindow()
{

}

void rRenderTest::finalize()
{

}

appMainEntry(rRenderTest)
