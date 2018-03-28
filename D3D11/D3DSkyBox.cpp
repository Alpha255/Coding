#include "Skybox.h"
//#include "Camera.h"
//
//extern D3DGraphic* g_Renderer;
//
//void SkyBox::Create(const char *pCubemapName, float skySphereRadius)
//{
//	assert(pCubemapName);
//
//	g_Renderer->CreateShaderResourceView(m_Cubemap, pCubemapName);
//
//	Math::Geometry::Mesh sphere;
//	Math::Geometry::MakeGeoSphere(skySphereRadius, 3U, sphere);
//
//	m_IndexCount = (uint32_t)sphere.Indices.size();
//
//	g_Renderer->CreateVertexBuffer(m_VertexBuffer, sizeof(Math::Geometry::Vertex) * (uint32_t)sphere.Vertices.size(), D3D11_USAGE_IMMUTABLE, &sphere.Vertices[0]);
//	g_Renderer->CreateIndexBuffer(m_IndexBuffer, sizeof(uint32_t) * m_IndexCount, D3D11_USAGE_IMMUTABLE, &sphere.Indices[0]);
//
//	static char * const s_ShaderName = "SkyBox.hlsl";
//
//	D3D11_INPUT_ELEMENT_DESC layout[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//	};
//	g_Renderer->CreateVertexShaderAndInputLayout(m_VertexShader, m_Layout, layout, ARRAYSIZE(layout), s_ShaderName, "VS_Main");
//
//	g_Renderer->CreatePixelShader(m_PixelShader, s_ShaderName, "PS_Main");
//
//	g_Renderer->CreateConstantBuffer(m_ConstansBuffer, sizeof(ConstantsBufferVS), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
//
//	D3D11_SAMPLER_DESC sampDesc;
//	memset(&sampDesc, 0, sizeof(D3D11_SAMPLER_DESC));
//	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
//	sampDesc.MinLOD = 0.0f;
//	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
//	g_Renderer->CreateSamplerState(m_Sampler, &sampDesc);
//
//	g_Renderer->CreateRasterizerState(m_NoBackFaceCulling, D3D11_FILL_SOLID, D3D11_CULL_NONE);
//
//	/// Make sure the depth function is LESS_EQUAL and not just LESS.  
//	/// Otherwise, the normalized depth values at z = 1 (NDC) will 
//	/// fail the depth test if the depth buffer was cleared to 1.
//	D3D11_DEPTH_STENCIL_DESC depthDesc = { 0 };
//	depthDesc.DepthEnable = true;
//	depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
//	g_Renderer->CreateDepthStencilState(m_DepthLess_Equal, &depthDesc);
//}
//
//void SkyBox::RestorState()
//{
//	static Ref<ID3D11RasterizerState> s_NullRasterizerState;
//	static Ref<ID3D11DepthStencilState> s_NullDepthStencilState;
//
//	g_Renderer->SetRasterizerState(s_NullRasterizerState);
//	///g_Renderer->SetSamplerStates(nullptr);
//	g_Renderer->SetDepthStencilState(s_NullDepthStencilState, 0U);
//}
//
//void SkyBox::Draw(const Camera &cam)
//{
//	ConstantsBufferVS cb;
//	Vec4 eyePos = cam.GetEyePos();
//	Matrix trans = Matrix::Translation(eyePos.x, eyePos.y, eyePos.z);
//	Matrix world = cam.GetWorldMatrix();
//	world *= trans;
//	Matrix wvp = world * cam.GetViewMatrix() * cam.GetProjMatrix();
//	cb.WVP = wvp.Transpose();
//
//	g_Renderer->SetRasterizerState(m_NoBackFaceCulling);
//	g_Renderer->SetDepthStencilState(m_DepthLess_Equal, 1U);
//
//	g_Renderer->SetInputLayout(m_Layout);
//
//	g_Renderer->SetVertexShader(m_VertexShader);
//	g_Renderer->SetPixelShader(m_PixelShader);
//
//	g_Renderer->SetVertexBuffer(m_VertexBuffer, sizeof(Math::Geometry::Vertex), 0U);
//	g_Renderer->SetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT);
//
//	g_Renderer->SetConstantBuffer(m_ConstansBuffer, 0U, D3DGraphic::eVertexShader);
//	g_Renderer->UpdateBuffer(m_ConstansBuffer, &cb, sizeof(ConstantsBufferVS));
//
//	g_Renderer->SetSamplerStates(m_Sampler, 0U, D3DGraphic::ePixelShader);
//
//	g_Renderer->SetShaderResource(m_Cubemap, 0U, D3DGraphic::ePixelShader);
//
//	g_Renderer->DrawIndexed(m_IndexCount, 0U, 0);
//
//	RestorState();
//}