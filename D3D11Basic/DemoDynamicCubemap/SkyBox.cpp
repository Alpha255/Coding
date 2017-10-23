#include "Skybox.h"
#include "Camera.h"

extern D3DGraphic* g_Renderer;

void SkyBox::Create(const char *pCubemapName, float skySphereRadius)
{
	assert(pCubemapName);

	g_Renderer->CreateShaderResourceView(m_Cubemap.Reference(), pCubemapName);

	Math::Geometry::Mesh sphere;
	Math::Geometry::MakeGeoSphere(skySphereRadius, 3U, sphere);

	m_IndexCount = (uint32_t)sphere.Indices.size();

	g_Renderer->CreateVertexBuffer(m_VertexBuffer.Reference(), sizeof(Math::Geometry::Vertex) * (uint32_t)sphere.Vertices.size(), D3D11_USAGE_IMMUTABLE, &sphere.Vertices[0]);
	g_Renderer->CreateIndexBuffer(m_IndexBuffer.Reference(), sizeof(uint32_t) * m_IndexCount, D3D11_USAGE_IMMUTABLE, &sphere.Indices[0]);

	static char * const s_ShaderName = "SkyBox.hlsl";

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	g_Renderer->CreateVertexShaderAndInputLayout(m_VertexShader.Reference(), m_Layout.Reference(), layout, ARRAYSIZE(layout), s_ShaderName, "VS_Main");

	g_Renderer->CreatePixelShader(m_PixelShader.Reference(), s_ShaderName, "PS_Main");

	g_Renderer->CreateConstantBuffer(m_ConstansBuffer.Reference(), sizeof(ConstantsBufferVS), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	D3D11_SAMPLER_DESC sampDesc;
	memset(&sampDesc, 0, sizeof(D3D11_SAMPLER_DESC));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0.0f;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	g_Renderer->CreateSamplerState(m_Sampler.Reference(), &sampDesc);
}

void SkyBox::Draw(const Camera &cam)
{
	g_Renderer->SetInputLayout(m_Layout.Ptr());

	g_Renderer->SetVertexShader(m_VertexShader.Ptr());
	g_Renderer->SetPixelShader(m_PixelShader.Ptr());

	g_Renderer->SetVertexBuffer(m_VertexBuffer.Ptr(), sizeof(Math::Geometry::Vertex), 0U);
	g_Renderer->SetIndexBuffer(m_IndexBuffer.Ptr(), DXGI_FORMAT_R32_UINT);

	ConstantsBufferVS cb;
	cb.WVP = cam.GetWorldMatrix() * cam.GetViewMatrix() * cam.GetProjMatrix();
	Vec4 eyePos = cam.GetEyePos();
	Matrix trans = Matrix::Translation(eyePos.x, eyePos.y, eyePos.z);
	///cb.WVP *= trans;
	cb.WVP = cb.WVP.Transpose();

	g_Renderer->SetConstantBuffer(m_ConstansBuffer.Ptr(), 0U, D3DGraphic::eVertexShader);
	g_Renderer->UpdateConstantBuffer(m_ConstansBuffer.Ptr(), &cb, sizeof(ConstantsBufferVS));

	g_Renderer->SetSamplerStates(m_Sampler.Reference());

	g_Renderer->SetShaderResource(m_Cubemap.Reference());

	g_Renderer->DrawIndexed(m_IndexCount, 0U, 0);
}