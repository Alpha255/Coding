#include "Colorful/D3D/D3D11/D3D11Engine.h"

D3D11GraphicsPipeline::D3D11GraphicsPipeline(const D3D11Device& device, const GfxPipelineState* state)
	: m_GfxState(state)
{
	assert(state);

	m_RasterizerState = std::make_shared<D3D11RasterizerState>(device, state->RasterizerStateDesc);
	m_BlendState = std::make_shared<D3D11BlendState>(device, state->BlendStateDesc);
	m_DepthStencilState = std::make_shared<D3D11DepthStencilState>(device, state->DepthStencilStateDesc);

	auto vertexShader = std::static_pointer_cast<D3D11Shader>(state->Material.Shaders[eVertexShader]);
	m_InputLayout = std::make_shared<D3D11InputLayout>(device, vertexShader->binary(), state->Material.InputLayout, state->VertexStrideAlignment);
}

void D3D11Context::setGraphicsPipeline(const D3D11GraphicsPipelinePtr& pipeline)
{
	assert(pipeline && pipeline->gfxPipelineState());
	auto gfxState = pipeline->gfxPipelineState();

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
		auto shader = gfxState->Material.Shaders[i];
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
	setShaderResources(pipeline);

	auto rasterizerState = pipeline->rasterizerState()->get();
	if (rasterizerState != m_State.RasterizerState)
	{
		m_State.RasterizerState = rasterizerState;
		m_State.setDirty(D3D11PipelineState::eDirtyTag::eRasterizerState, true);
	}

	auto depthStencilState = pipeline->depthStencilState()->get();
	if (depthStencilState != m_State.DepthStencilState)
	{
		m_State.DepthStencilState = depthStencilState;
		m_State.setDirty(D3D11PipelineState::eDirtyTag::eDepthStencilState, true);
	}

	auto blendState = pipeline->blendState()->get();
	if (blendState != m_State.BlendState)
	{
		m_State.BlendState = blendState;
		m_State.setDirty(D3D11PipelineState::eDirtyTag::eBlendState, true);
	}

	/// set rendertarget/depthstencil

	/// set viewport/scissor

	m_State.submit(*this);

#undef ShaderCaster
}

void D3D11Context::setVertexIndexBuffers(const D3D11GraphicsPipelinePtr& pipeline)
{
	auto gfxState = pipeline->gfxPipelineState();

	/// set input layout
	auto inputLayout = pipeline->inputLayout();
	if (inputLayout->get() != m_State.InputLayout)
	{
		m_State.InputLayout = inputLayout->get();
		m_State.setDirty(D3D11PipelineState::eDirtyTag::eInputLayout, true);
	}

	auto primitiveTopology = D3D11Enum::toPrimitiveTopology(gfxState->PrimitiveTopology);
	if (primitiveTopology != m_State.PrimitiveTopology)
	{
		m_State.PrimitiveTopology = primitiveTopology;
		m_State.setDirty(D3D11PipelineState::eDirtyTag::ePrimitiveTopology, true);
	}

	auto vertexBuffer = static_cast<D3D11BufferPtr>(gfxState->VertexBuffer)->get();
	if (vertexBuffer != m_State.VertexBuffers[0u])
	{
		m_State.VertexBuffers[0u] = vertexBuffer;
		m_State.VertexOffsets[0u] = 0u;  /// ??? 
		m_State.VertexStrides[0u] = inputLayout->stride();
		
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

void D3D11Context::setShaderResources(const D3D11GraphicsPipelinePtr& pipeline)
{
	auto& resourceMap = pipeline->gfxPipelineState()->ResourceMap;

	uint32_t maxBinding = 0u;
	for (auto& res : resourceMap[eVertexShader])
	{
		maxBinding = std::max<uint32_t>(maxBinding, res.Binding);
	}

	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		auto& resources = resourceMap[i];
		for (auto& res : resources)
		{
			uint32_t slot = i > eVertexShader ? res.Binding - maxBinding : res.Binding;
			switch (res.Type)
			{
			case GfxPipelineState::eTexture:
			{
				auto texture = std::static_pointer_cast<D3D11ShaderResourceView>(res.Texture)->get();
				assert(texture);
				if (m_State.ShaderResourceViews[i][slot] != texture)
				{
					m_State.ShaderResourceViews[i][slot] = texture;
					m_State.ShaderResourceViewsInUse[i][slot] = 1u;
					m_State.ShaderResourceViewMaxSlot[i] = std::max<uint32_t>(m_State.ShaderResourceViewMaxSlot[i], slot);
					m_State.setDirty(D3D11PipelineState::eDirtyTag::eShaderResourceView, true, i);
				}
			}
				break;
			case GfxPipelineState::eSampler:
			{
				auto sampler = std::static_pointer_cast<D3D11SamplerState>(res.Sampler)->get();
				assert(sampler);
				if (m_State.Samplers[i][slot] != sampler)
				{
					m_State.Samplers[i][slot] = sampler;
					m_State.SamplersInUse[i][slot] = 1u;
					m_State.SamplerMaxSlot[i] = std::max<uint32_t>(m_State.SamplerMaxSlot[i], slot);
					m_State.setDirty(D3D11PipelineState::eDirtyTag::eSampler, true, i);
				}
			}
				break;
			case GfxPipelineState::eCombinedTextureSampler:
			{
				auto texture = std::static_pointer_cast<D3D11ShaderResourceView>(res.Texture)->get();
				assert(texture);
				if (m_State.ShaderResourceViews[i][slot] != texture)
				{
					m_State.ShaderResourceViews[i][slot] = texture;
					m_State.ShaderResourceViewsInUse[i][slot] = 1u;
					m_State.ShaderResourceViewMaxSlot[i] = std::max<uint32_t>(m_State.ShaderResourceViewMaxSlot[i], slot);
					m_State.setDirty(D3D11PipelineState::eDirtyTag::eShaderResourceView, true, i);
				}

				auto sampler = std::static_pointer_cast<D3D11SamplerState>(res.Sampler)->get();
				assert(sampler);
				if (m_State.Samplers[i][slot] != sampler)
				{
					m_State.Samplers[i][slot] = sampler;
					m_State.SamplersInUse[i][slot] = 1u;
					m_State.SamplerMaxSlot[i] = std::max<uint32_t>(m_State.SamplerMaxSlot[i], slot);
					m_State.setDirty(D3D11PipelineState::eDirtyTag::eSampler, true, i);
				}
			}
				break;
			case GfxPipelineState::eUniformBuffer:
			{
				auto uniformBuffer = (static_cast<D3D11BufferPtr>(res.UniformBuffer))->get();
				assert(uniformBuffer);
				if (m_State.UniformBuffers[i][slot] != uniformBuffer)
				{
					m_State.UniformBuffers[i][slot] = uniformBuffer;
					m_State.UniformBuffersInUse[i][slot] = 1u;
					m_State.UniformBufferMaxSlot[i] = std::max<uint32_t>(m_State.UniformBufferMaxSlot[i], slot);
					m_State.setDirty(D3D11PipelineState::eDirtyTag::eUniformBuffer, true, i);
				}
			}
				break;
			}
		}
	}
}
