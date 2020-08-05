#pragma once

#include "Colorful/Public/GfxDefinitions.h"

struct D3D11PipelineState
{
public:
	enum eResourceLimits
	{
		eViewportLimits = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
		eScissorRectLimits = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
		eSamplerLimits = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT,
		eUniformBufferLimits = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT,
		eRenderTargetLimits = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
		eShaderResourceViewLimits = D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT,
		eUnorderedAccessViewLimits = D3D11_PS_CS_UAV_REGISTER_COUNT,  /// D3D11_1_UAV_SLOT_COUNT
		eVertexStreamLimits = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT  /// Custom
	};

	enum class eDirtyTag
	{
		eInputLayout,
		eVertexBuffer,
		eIndexBuffer,
		eUniformBuffer,
		eVertexShader = eUniformBuffer + eRShaderUsage_MaxEnum,
		eHullShader,
		eDomainShader,
		eFragmentShader,
		eGeometryShader,
		eComputeShader,
		eRasterizerState,
		eSampler,
		eBlendState = eSampler + eRShaderUsage_MaxEnum,
		eDepthStencilState,
		eRenderTargetView,
		eDepthStencilView,
		eShaderResourceView,
		eUnorderedAccessView = eShaderResourceView + eRShaderUsage_MaxEnum,
		eScissorRect,
		eViewport,
		ePrimitiveTopology,
		eDirtyTagCount
	};

	inline bool8_t isDirty(eDirtyTag tag, uint32_t index = 0u) const
	{
		return DirtyFlags[(uint32_t)tag + index];
	}

	inline void setDirty(eDirtyTag tag, bool8_t dirty, uint32_t index = 0U)
	{
		DirtyFlags[(uint32_t)tag + index] = dirty;
	}

	void submit(class D3D11Context& context);

	inline void clear()
	{
		memset(this, 0, sizeof(D3D11PipelineState));
		DirtyFlags.reset();
	}
protected:
	void setUniformBuffers(class D3D11Context& context);
	void setSamplers(class D3D11Context& context);
	void setShaderResourceViews(class D3D11Context& context);
	void setUnorderedAccessViews(class D3D11Context& context);
private:
	std::bitset<(uint32_t)eDirtyTag::eDirtyTagCount> DirtyFlags;

	D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	uint32_t StencilRef = 0u;
	uint32_t BlendMask = 0u;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_UNKNOWN;
	uint32_t IndexOffset = 0u;
	Vec4 BlendFactor;

	ID3D11InputLayout* InputLayout = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;

	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11HullShader* HullShader = nullptr;
	ID3D11DomainShader* DomainShader = nullptr;
	ID3D11PixelShader* FragmentShader = nullptr;
	ID3D11GeometryShader* GeometryShader = nullptr;
	ID3D11ComputeShader* ComputeShader = nullptr;

	ID3D11RasterizerState* RasterizerState = nullptr;
	ID3D11BlendState* BlendState = nullptr;
	ID3D11DepthStencilState* DepthStencilState = nullptr;

	ID3D11DepthStencilView* DepthStencilView = nullptr;

	std::array<uint32_t, eVertexStreamLimits> VertexStrides;
	std::array<uint32_t, eVertexStreamLimits> VertexOffsets;

	std::bitset<eVertexStreamLimits> VertexBuffersInUse;
	std::bitset<eViewportLimits> ViewportsInUse;
	std::bitset<eScissorRectLimits> ScissorRectsInUse;
	std::array<std::bitset<eUniformBufferLimits>, eRShaderUsage_MaxEnum> UniformBuffersInUse;
	std::array<std::bitset<eSamplerLimits>, eRShaderUsage_MaxEnum> SamplersInUse;
	std::array<std::bitset<eShaderResourceViewLimits>, eRShaderUsage_MaxEnum> ShaderResourceViewsInUse;
	std::bitset<eRenderTargetLimits> RenderTargetsInUse;
	std::bitset<eUnorderedAccessViewLimits> UnorderedAccessViewsInUse;

	std::array<ID3D11Buffer*, eVertexStreamLimits> VertexBuffers;
	std::array<D3D11_VIEWPORT, eViewportLimits> Viewports;
	std::array<D3D11_RECT, eScissorRectLimits> ScissorRects;
	std::array<std::array<ID3D11Buffer*, eUniformBufferLimits>, eRShaderUsage_MaxEnum> UniformBuffers;
	std::array<std::array<ID3D11SamplerState*, eSamplerLimits>, eRShaderUsage_MaxEnum> Samplers;
	std::array<std::array<ID3D11ShaderResourceView*, eShaderResourceViewLimits>, eRShaderUsage_MaxEnum> ShaderResourceViews;
	std::array<ID3D11RenderTargetView*, eRenderTargetLimits> RenderTargets;
	std::array<ID3D11UnorderedAccessView*, eUnorderedAccessViewLimits> UnorderedAccessViews;
};
