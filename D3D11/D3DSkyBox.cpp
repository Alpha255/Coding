#include "D3DSkybox.h"
#include "D3DEngine.h"
#include "Camera.h"

void D3DSkyBox::Create(const char *pCubemapName, float skySphereRadius)
{
	assert(pCubemapName);

	m_Resource.Cubemap.Create(pCubemapName);

	m_Sphere.CreateAsGeoSphere(skySphereRadius, 3U);

	/// Make sure the depth function is LESS_EQUAL and not just LESS.  
	/// Otherwise, the normalized depth values at z = 1 (NDC) will 
	/// fail the depth test if the depth buffer was cleared to 1.
	m_Resource.DepthLessEqual.Create(
		true, D3DState::eDepthMaskAll, D3DState::eLessEqual,
		false, D3DState::eStencilDefaultReadMask, D3DState::eStencilDefaultWriteMask,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eAlways,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eAlways);
}

void D3DSkyBox::Draw(const Camera &cam)
{
	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidNoneCulling);
	D3DEngine::Instance().SetDepthStencilState(m_Resource.DepthLessEqual, 1U);

	m_Sphere.Draw(cam);

	D3DEngine::Instance().SetRasterizerState(D3DStaticState::Solid);
	D3DEngine::Instance().SetDepthStencilState(D3DStaticState::NoneDepthStencilState, 1U);
}