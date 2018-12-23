#pragma once

#include "IImGUI.h"
#include "D3DEngine.h"

class D3D11ImGUI : public IImGUI
{
public:
	virtual void InitRenderResource(ImGuiIO &io) override;
	virtual void SetupRenderResource(Matrix &matrix) override;
	virtual void Draw(::RECT &rect, const ImDrawCmd *pDrawCmd, uint32_t indexOffset, int32_t startVertex) override;
	virtual void RestoreRenderResource(::RECT &rect) override;
	virtual void ResetVertexBuffer() override;
	virtual void ResetIndexBuffer() override;
	virtual void UpdateBuffers() override;
protected:
private:
	struct RenderResource
	{
		D3DInputLayout VertexLayout;

		D3DVertexShader VertexShader;
		D3DPixelShader PixelShader;

		D3DBuffer VertexBuffer;
		D3DBuffer IndexBuffer;
		D3DBuffer ConstantBufferVS;

		D3DBlendState ClrWriteBlend;

		D3DShaderResourceView FontTexture;
	};

	RenderResource m_Resource;
};
