#pragma once

#include "Colorful/D3D/DXGI_Interface.h"

#include <d3d12.h>

NAMESPACE_START(Gfx)

class D3D12Debug final : public D3DObject<ID3D12Debug>
{
public:
	D3D12Debug();
};
DECLARE_UNIQUE_PTR(D3D12Debug)

class D3D12Device final : public D3DObject<ID3D12Device>, public GfxDevice
{
public:
	D3D12Device(IDXGIAdapter1* adapter1, IDXGIAdapter4* adapter4);

	ShaderPtr createVertexShader(ShaderDesc& desc) override final
	{
		(void)desc;
		return nullptr;
	}
	ShaderPtr createHullShader(ShaderDesc& desc) override final
	{
		(void)desc;
		return nullptr;
	}
	ShaderPtr createDomainShader(ShaderDesc& desc) override final
	{
		(void)desc;
		return nullptr;
	}
	ShaderPtr createGeometryShader(ShaderDesc& desc) override final
	{
		(void)desc;
		return nullptr;
	}
	ShaderPtr createFragmentShader(ShaderDesc& desc) override final
	{
		(void)desc;
		return nullptr;
	}
	ShaderPtr createComputeShader(ShaderDesc& desc) override final
	{
		(void)desc;
		return nullptr;
	}

	TexturePtr create1DTexture(const TextureDesc& desc) override final
	{
		(void)desc;
		return nullptr;
	}
	TexturePtr create2DTexture(const TextureDesc& desc) override final
	{
		(void)desc;
		return nullptr;
	}
	TexturePtr create3DTexture(const TextureDesc& desc) override final
	{
		(void)desc;
		return nullptr;
	}

	TexturePtr createRenderTarget(const TextureDesc& desc) override final
	{
		(void)desc;
		return nullptr;
	}

	TexturePtr createDepthStencil(const TextureDesc& desc) override final
	{
		(void)desc;
		return nullptr;
	}

	InputLayoutPtr createInputLayout(const VertexInputLayoutDesc& desc, const ShaderDesc& binary) override final
	{
		(void)desc;
		(void)binary;
		return nullptr;
	}

	GpuBufferPtr createUniformBuffer(size_t size, const void* data) override final
	{
		(void)size;
		(void)data;
		return nullptr;
	}
	GpuBufferPtr createIndexBuffer(EBufferUsage usage, size_t size, const void* data) override final
	{
		(void)usage;
		(void)size;
		(void)data;
		return nullptr;
	}
	GpuBufferPtr createVertexBuffer(EBufferUsage usage, size_t size, const void* data) override final
	{
		(void)usage;
		(void)size;
		(void)data;
		return nullptr;
	}

	CommandBufferPtr getActiveCommandBuffer() override final
	{
		return nullptr;
	}
protected:
	SamplerPtr createSampler_native(const SamplerDesc& desc) override final
	{
		(void)desc;
		return nullptr;
	}
private:
};
DECLARE_UNIQUE_PTR(D3D12Device)

NAMESPACE_END(Gfx)
