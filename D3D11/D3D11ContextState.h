#pragma once

#include "D3D11Object.h"

struct D3D11ContextState
{	
public:
	enum eResourceLimits
	{
		eMaxViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
		eMaxScissorRects = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
		eMaxSamplers = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT,
		eMaxConstantBuffers = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT,
		eMaxRenderTargetViews = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
		eMaxShaderResourceViews = D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT / 16U,  /// Don't need so many
		eMaxUnorderedAccessViews = D3D11_PS_CS_UAV_REGISTER_COUNT,  /// D3D11_1_UAV_SLOT_COUNT
		eMaxVertexStream = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT  /// Custom
	};

	enum eDirtyFlag
	{
		eInputLayout,
		eVertexBuffer,
		eIndexBuffer,

		eConstantBuffer,

		eVertexShader = eConstantBuffer + eRShaderTypeCount,
		eHullShader,
		eDomainShader,
		ePixelShader,
		eGeometryShader,
		eComputeShader,

		eRasterizerState,

		eSamplerState,
		
		eBlendState = eSamplerState + eRShaderTypeCount,
		eDepthStencilState,

		eRenderTargetView,
		eDepthStencilView,

		eShaderResourceView,

		eUnorderedAccessView = eShaderResourceView + eRShaderTypeCount,

		eScissorRect,
		eViewport,

		ePrimitiveTopology,

		eDirtyFlagCount
	};

	static_assert(eDirtyFlagCount <= sizeof(uint64_t) * 8U, "Out of range!!!");

	inline bool IsDirty(eDirtyFlag flag, uint32_t index = 0U) const
	{
		return ((DirtyFlag) & ((uint64_t)1 << (flag + index))) != 0U;
	}

	inline void SetDirty(eDirtyFlag flag, bool bDirty, uint32_t index = 0U)
	{
		DirtyFlag = bDirty ? (DirtyFlag | ((uint64_t)1 << (flag + index))) : (DirtyFlag & ~((uint64_t)1 << (flag + index)));
	}

	void CommitState(class D3D11Context &ctx);

	inline void ClearState()
	{
		memset(this, 0, sizeof(D3D11ContextState));
	}

	friend class D3D11Context;

protected:
	void BindConstantBuffers(class D3D11Context &ctx);
	void BindSamplerStates(class D3D11Context &ctx);
	void BindShaderResourceViews(class D3D11Context &ctx);
	void BindUnorderedAccessViews(class D3D11Context &ctx);

	template<typename T> inline uint32_t GetCount(T counter)
	{
		uint32_t count = 0U;
		while (counter)
		{
			counter = counter & (counter - 1);
			++count;
		}

		return count;
	}

private:
	uint64_t                 DirtyFlag = 0U;

	uint32_t                 PrimitiveTopology = UINT32_MAX;
	uint32_t                 StencilRef = 0U;
	uint32_t                 BlendMask = 0U;
	uint32_t                 IndexFormat = 0U;
	uint32_t                 IndexOffset = 0U;

	Vec4                     BlendFactor;

	ID3D11InputLayout        *InputLayout = nullptr;
	ID3D11Buffer             *IndexBuffer = nullptr;

	ID3D11VertexShader       *VertexShader = nullptr;
	ID3D11HullShader         *HullShader = nullptr;
	ID3D11DomainShader       *DomainShader = nullptr;
	ID3D11PixelShader        *PixelShader = nullptr;
	ID3D11GeometryShader     *GeometryShader = nullptr;
	ID3D11ComputeShader      *ComputeShader = nullptr;

	ID3D11RasterizerState    *RasterizerState = nullptr;
	ID3D11BlendState         *BlendState = nullptr;
	ID3D11DepthStencilState  *DepthStencilState = nullptr;

	ID3D11DepthStencilView   *DepthStencilView = nullptr;

	std::array<uint32_t, eMaxVertexStream> VertexStrides;
	std::array<uint32_t, eMaxVertexStream> VertexOffsets;

	std::array<ID3D11Buffer *, eMaxVertexStream> VertexBuffers;
	uint32_t VertexBuffersInUse = 0U;  

	std::array<D3D11_VIEWPORT, eMaxViewports> Viewports;
	uint16_t ViewportsInUse = 0U;

	std::array<D3D11_RECT, eMaxScissorRects> ScissorRects;
	uint16_t ScissorRectsInUse = 0U;

	std::array<std::array<ID3D11Buffer *, eMaxConstantBuffers>, eRShaderTypeCount> ConstantBuffers;
	uint16_t ConstantBuffersInUse[eRShaderTypeCount] = {};

	std::array<std::array<ID3D11SamplerState *, eMaxSamplers>, eRShaderTypeCount> SamplerStates;
	uint16_t SamplersInUse[eRShaderTypeCount] = {};

	std::array<std::array<ID3D11ShaderResourceView *, eMaxShaderResourceViews>, eRShaderTypeCount> ShaderResourceViews;
	uint8_t ShaderResourceViewsInUse[eRShaderTypeCount] = {};

	std::array<ID3D11RenderTargetView *, eMaxRenderTargetViews> RenderTargetViews;
	uint8_t RenderTargetsInUse = 0U;

	std::array<ID3D11UnorderedAccessView *, eMaxUnorderedAccessViews> UnorderedAccessViews;
	uint8_t UnorderedAccessViewsInUse = 0U;
};
