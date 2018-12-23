#include "D3D11ImGUI.h"

void D3D11ImGUI::InitRenderResource(ImGuiIO &io)
{
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_Resource.VertexShader.Create("imGUI.hlsl", "VS_Main");
	m_Resource.VertexLayout.Create(m_Resource.VertexShader.GetBlob(), layout, _countof(layout));
	m_Resource.PixelShader.Create("imGUI.hlsl", "PS_Main");

	m_Resource.ConstantBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite, nullptr);

	m_Resource.ClrWriteBlend.Create(false, false, 0U, true, D3DState::eSrcAlpha, D3DState::eInvSrcAlpha, D3DState::eAdd,
		D3DState::eInvSrcAlpha, D3DState::eZero, D3DState::eAdd, D3DState::eColorAll);

	unsigned char *pPixels = nullptr;
	int32_t width = 0, height = 0;
	io.Fonts->GetTexDataAsRGBA32(&pPixels, &width, &height);

	D3DTexture2D fontTex;
	D3D11_SUBRESOURCE_DATA subRes = {};
	subRes.pSysMem = (const void *)pPixels;
	subRes.SysMemPitch = (uint32_t)width * 4U;
	fontTex.Create(eRGBA8_UNorm, (uint32_t)width, (uint32_t)height, D3DBuffer::eBindAsShaderResource, 1U, 1U, 0U, 0U,
		D3DBuffer::eGpuReadWrite, &subRes);
	m_Resource.FontTexture.CreateAsTexture(D3DView::eTexture2D, fontTex, eRGBA8_UNorm, 0U, 1U);
	io.Fonts->TexID = (void *)m_Resource.FontTexture.Get();
}

void D3D11ImGUI::SetupRenderResource(Matrix &matrix)
{
	m_Resource.ConstantBufferVS.Update(&matrix, sizeof(Matrix));

	D3DViewport vp(0.0f, 0.0f, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

	D3DEngine::Instance().SetViewport(vp);
	D3DEngine::Instance().SetVertexShader(m_Resource.VertexShader);
	D3DEngine::Instance().SetPixelShader(m_Resource.PixelShader);
	D3DEngine::Instance().SetInputLayout(m_Resource.VertexLayout);
	D3DEngine::Instance().SetVertexBuffer(m_Resource.VertexBuffer, sizeof(ImDrawVert), 0U);
	D3DEngine::Instance().SetIndexBuffer(m_Resource.IndexBuffer, sizeof(ImDrawIdx) == 2U ? eR16_UInt : eR32_UInt, 0U);
	D3DEngine::Instance().SetConstantBuffer(m_Resource.ConstantBufferVS, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetBlendState(m_Resource.ClrWriteBlend);
	D3DEngine::Instance().SetDepthStencilState(D3DStaticState::DisableDepthStencil, 0U);
	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidNoneCulling);
}

void D3D11ImGUI::Draw(::RECT &rect, const ImDrawCmd *pDrawCmd, uint32_t indexOffset, int32_t startVertex)
{
	D3DEngine::Instance().SetShaderResourceView(m_Resource.FontTexture, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetScissorRect(rect);
	D3DEngine::Instance().DrawIndexed(pDrawCmd->ElemCount, indexOffset, startVertex, eTriangleList);
}

void D3D11ImGUI::RestoreRenderResource(::RECT &rect)
{
	D3DViewport vpOld(0.0f, 0.0f, (float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

	D3DEngine::Instance().SetViewport(vpOld);
	D3DEngine::Instance().SetScissorRect(rect);
	D3DEngine::Instance().SetRasterizerState(D3DStaticState::Solid);
	D3DEngine::Instance().SetBlendState(D3DStaticState::NoneBlendState);
	D3DEngine::Instance().SetDepthStencilState(D3DStaticState::NoneDepthStencilState, 0U);
}

void D3D11ImGUI::ResetVertexBuffer() 
{
	m_Resource.VertexBuffer.Reset();
	m_Resource.VertexBuffer.CreateAsVertexBuffer(m_VertexCount * sizeof(ImDrawVert), D3DBuffer::eGpuReadCpuWrite, nullptr);
}
void D3D11ImGUI::ResetIndexBuffer()
{
	m_Resource.IndexBuffer.Reset();
	m_Resource.IndexBuffer.CreateAsIndexBuffer(m_IndexCount * sizeof(ImDrawIdx), D3DBuffer::eGpuReadCpuWrite, nullptr);
}

void D3D11ImGUI::UpdateBuffers()
{
	m_Resource.VertexBuffer.Update(m_Vertices.get(), sizeof(ImDrawVert) * m_VertexCount);
	m_Resource.IndexBuffer.Update(m_Indices.get(), sizeof(ImDrawIdx) * m_IndexCount);
}