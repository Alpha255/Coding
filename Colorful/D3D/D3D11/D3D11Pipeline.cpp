#include "Colorful/D3D/D3D11/D3D11Engine.h"

D3D11GraphicsPipeline::D3D11GraphicsPipeline(const D3D11Device& device, const GfxPipelineState* state)
	: m_GfxState(state)
{
	assert(state);

	m_RasterizerState = std::make_shared<D3D11RasterizerState>(device, state->RasterizerStateDesc);
	m_BlendState = std::make_shared<D3D11BlendState>(device, state->BlendStateDesc);
	m_DepthStencilState = std::make_shared<D3D11DepthStencilState>(device, state->DepthStencilStateDesc);
}

void D3D11Context::setGraphicsPipeline(const D3D11GraphicsPipelinePtr& pipeline)
{
	assert(pipeline && pipeline->gfxPipelineState());

#define ShaderCaster(Category)                                                                            \
{                                                                                                         \
	auto Category = static_cast<ID3D11##Category*>(std::static_pointer_cast<D3D11Shader>(shader)->get()); \
	if (Category != m_State.Category)                                                                     \
	{                                                                                                     \
		m_State.Category = Category;                                                                      \
		m_State.setDirty(D3D11PipelineState::eDirtyTag::e##Category, true);                               \
	}                                                                                                     \
}

	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		auto shader = pipeline->gfxPipelineState()->Material.Shaders[i];
		if (shader)
		{
			switch (i)
			{
			case eVertexShader:
				ShaderCaster(VertexShader)
				break;
			case eHullShader:
				ShaderCaster(HullShader)
				break;
			case eDomainShader:
				ShaderCaster(DomainShader)
				break;
			case eGeometryShader:
				ShaderCaster(GeometryShader)
				break;
			case eFragmentShader:
				ShaderCaster(PixelShader)
				break;
			case eComputeShader:
				ShaderCaster(ComputeShader)
				break;
			}
		}
	}

	setVertexIndexBuffers(pipeline);

#undef ShaderCaster
}

void D3D11Context::setVertexIndexBuffers(const D3D11GraphicsPipelinePtr& pipeline)
{
	auto gfxState = pipeline->gfxPipelineState();

	/// set input layout

	auto vertexBuffer = static_cast<D3D11BufferPtr>(gfxState->VertexBuffer)->get();
	if (vertexBuffer != m_State.VertexBuffers[0u])
	{
		m_State.VertexBuffers[0u] = vertexBuffer;
		m_State.VertexOffsets[0u] = 0u;
		m_State.VertexStrides[0u] = 0u; /// ??? 
		
		m_State.VertexBuffersInUse[0] = 1u;
		m_State.setDirty(D3D11PipelineState::eDirtyTag::eVertexBuffer, true);
	}

	auto indexBuffer = static_cast<D3D11BufferPtr>(gfxState->IndexBuffer)->get();
	if (indexBuffer != m_State.IndexBuffer)
	{
		m_State.IndexBuffer = indexBuffer;
		m_State.IndexOffset = 0u; /// ???
		m_State.IndexFormat = gfxState->IndexType == eRIndexType::eUInt16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

		m_State.setDirty(D3D11PipelineState::eDirtyTag::eIndexBuffer, true);
	}
}
