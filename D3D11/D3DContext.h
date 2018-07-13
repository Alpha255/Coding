#pragma once

#include "D3DShader.h"
#include "D3DMath.h"

class D3DContext : public D3DObject<ID3D11DeviceContext>
{
public:	
	void CommitState();
protected:
	friend class D3DEngine;
	
	enum eResourceLimits
	{
		eMaxViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
		eMaxScissorRects = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
		eMaxSamplers = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT,
		eMaxConstantBuffers = D3D11_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT,
		eMaxRenderTargetViews = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
		eMaxShaderResourceView = D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT / 4U,  /// Don't need so many
		eMaxUnorderedAccessViews = D3D11_PS_CS_UAV_REGISTER_COUNT,  /// D3D11_1_UAV_SLOT_COUNT
		eMaxVertexStream = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT  /// Custom
	};

	enum eDirtyFlag
	{
		eDInputLayout,
		eDVertexBuffer,
		eDIndexBuffer,
		eDConstantBuffer,

		eDVertexShader,
		eDHullShader,
		eDDomainShader,
		eDPixelShader,
		eDGeometryShader,
		eDComputeShader,

		eDRasterizerState,
		eDSamplerState,
		eDBlendState,
		eDDepthStencilState,

		eDRenderTargetView,
		eDDepthStencilView,
		eDShaderResourceView,
		eDUnorderedAccessView,

		eDScissorRect,
		eDViewport,

		eDPrimitiveTopology,

		eDirtyFlagCount
	};

	static_assert(eDirtyFlagCount <= sizeof(uint32_t) * 8U, "Out of range!!!");

	struct D3DContextState
	{
		uint32_t                 DirtyFlag = 0U;

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

		std::array<std::array<ID3D11Buffer *, eMaxConstantBuffers>, D3DShader::eShaderTypeCount> ConstantBuffers;
		uint16_t ConstantBuffersInUse[D3DShader::eShaderTypeCount] = {};

		std::array<std::array<ID3D11SamplerState *, eMaxSamplers>, D3DShader::eShaderTypeCount> SamplerStates;
		uint16_t SamplersInUse[D3DShader::eShaderTypeCount] = {};

		std::array<std::array<ID3D11ShaderResourceView *, eMaxShaderResourceView>, D3DShader::eShaderTypeCount> ShaderResourceViews;
		uint32_t ShaderResourceViewsInUse[D3DShader::eShaderTypeCount] = {};

		std::array<ID3D11RenderTargetView *, eMaxRenderTargetViews> RenderTargetViews;
		uint8_t RenderTargetsInUse = 0U;

		std::array<ID3D11UnorderedAccessView *, eMaxUnorderedAccessViews> UnorderedAccessViews;
		uint8_t UnorderedAccessViewsInUse = 0U;

		inline bool IsDirty(eDirtyFlag flag) const
		{
			return (DirtyFlag & (1 << flag)) != 0U;
		}

		inline void SetDirty(eDirtyFlag flag, bool bDirty)
		{
			DirtyFlag = bDirty ? (DirtyFlag | (1 << flag)) : (DirtyFlag & ~(1 << flag));
		}

		inline void ResetState()
		{
			VertexBuffersInUse = 0U;
			ViewportsInUse = 0U;
			ScissorRectsInUse = 0U;
			ConstantBuffersInUse[D3DShader::eShaderTypeCount] = {};
			SamplersInUse[D3DShader::eShaderTypeCount] = {};
			ShaderResourceViewsInUse[D3DShader::eShaderTypeCount] = {};
			RenderTargetsInUse = 0U;
			UnorderedAccessViewsInUse = 0U;
		}
	};

	template <typename T> uint32_t GetCount(T recoder) const 
	{
		uint32_t count = 0U;
		while (recoder)
		{
			recoder = recoder & (recoder - 1);
			++count;
		}

		return count;
	}

	void BindConstantBuffers();
	void BindSamplerStates();
	void BindShaderResourceViews();
	void BindUnorderedAccessViews();
private:
	D3DContextState State;
};
