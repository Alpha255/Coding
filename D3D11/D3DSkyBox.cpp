#include "D3DSkybox.h"
#include "D3DEngine.h"
#include "Camera.h"

void D3DSkyBox::Create(const char *pCubemapName, float skySphereRadius)
{
	assert(pCubemapName && !m_Inited);

	m_Cubemap.Create(pCubemapName);

	m_SphereMesh.CreateAsGeoSphere(skySphereRadius, 3U);

	m_VertexShader.Create("SkyBox.hlsl", "VSMain");
	m_PixelShader.Create("SkyBox.hlsl", "PSMain");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite, nullptr);

	/// Make sure the depth function is LESS_EQUAL and not just LESS.  
	/// Otherwise, the normalized depth values at z = 1 (NDC) will 
	/// fail the depth test if the depth buffer was cleared to 1.
	m_DepthLessEqual.Create(
		true, D3DState::eDepthMaskAll, D3DState::eLessEqual,
		false, D3DState::eStencilDefaultReadMask, D3DState::eStencilDefaultWriteMask,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eAlways,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eAlways);

	m_Inited = true;
}

void D3DSkyBox::Draw(const Camera &cam)
{
	assert(m_Inited);

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetInputLayout(m_SphereMesh.VertexLayout);
	D3DEngine::Instance().SetVertexBuffer(m_SphereMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U);
	D3DEngine::Instance().SetIndexBuffer(m_SphereMesh.IndexBuffer, eR32_UInt, 0U);

	Vec4 eyePos = cam.GetEyePos();
	Matrix translation = Matrix::Translation(eyePos.x, eyePos.y, eyePos.z);
	Matrix wvp = Matrix::Transpose(cam.GetWorldMatrix() * translation * cam.GetViewMatrix() * cam.GetProjMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	D3DEngine::Instance().SetShaderResourceView(m_Cubemap, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidNoneCulling);
	D3DEngine::Instance().SetDepthStencilState(m_DepthLessEqual, 0xffffffff);

	D3DEngine::Instance().DrawIndexed(m_SphereMesh.IndexCount, 0U, 0, eTriangleList);

	D3DEngine::Instance().SetRasterizerState(D3DStaticState::Solid);
	D3DEngine::Instance().SetDepthStencilState(D3DStaticState::NoneDepthStencilState, 0U);
}