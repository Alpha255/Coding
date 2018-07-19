#include "LightController.h"
#include "D3DGeometryBuffer.h"
#include "D3DEngine.h"

void LightController::Init()
{
	m_DirLight.Diffuse = Vec4(0.85f, 0.8f, 0.5f, 0.0f);
	m_DirLight.Direction = Vec4(1.0f, -1.0f, 1.0f, 0.0f);
	m_DirLight.Specular = Vec3(0.25f, 0.25f, 0.25f);
	m_DirLight.SpecularIntensity = 250.0f;

	m_NoDepthWriteLessStencilMask.Create(
		true, D3DState::eDepthMaskZero, D3DState::eLess,
		true, D3DState::eStencilDefaultReadMask, D3DState::eStencilDefaultWriteMask,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eEqual,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eEqual
	);

	m_VertexShader.Create("DeferredShading\\Lighting.hlsl", "VSMain");
	m_PixelShader.Create("DeferredShading\\Lighting.hlsl", "PSMain");

	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite);

	m_Inited = true;
}

void LightController::PointLighting(const D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix)
{
}

void LightController::DirectionalLighting(const D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix)
{
	D3DEngine::Instance().SetDepthStencilState(m_NoDepthWriteLessStencilMask, 0x01U);

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DInputLayout EmptyLayout;
	D3DEngine::Instance().SetInputLayout(EmptyLayout);

	D3DBuffer EmptyBuffer;
	D3DEngine::Instance().SetVertexBuffer(EmptyBuffer, 0U, 0U, 0U);

	ConstantBufferPS CBufferPS;
	CBufferPS.AmbientLowerClr = m_AmbientLowerClr;
	CBufferPS.AmbientRange = m_AmbientUpperClr - m_AmbientLowerClr;
	CBufferPS.DirLight = m_DirLight;
	CBufferPS.PerspectiveValue = camPerspective;
	CBufferPS.ViewMatirxInvese = Matrix::InverseTranspose(camViewMatrix);
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	for (uint32_t i = 0U; i < D3DGeometryBuffer::eBufferTypeCount; ++i)
	{
		D3DEngine::Instance().SetShaderResourceView(GBuffer.GetShaderResourceView((D3DGeometryBuffer::eBufferType)i), i, D3DShader::ePixelShader);
	}

	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().Draw(4U, 0U, eTriangleStrip);
}

void LightController::SpotLighting(const D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix)
{
}

void LightController::CapsuleLighting(const D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix)
{
}
