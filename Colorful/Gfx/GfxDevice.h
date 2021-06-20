#pragma once

#include "Colorful/Gfx/GfxRenderState.h"
#include "Colorful/Gfx/GfxResourceFactory.h"
#include "Colorful/Gfx/GfxCommand.h"

NAMESPACE_START(Gfx)

class GfxDevice
{
public:
	virtual ShaderPtr createVertexShader(ShaderDesc& desc) = 0;
	virtual ShaderPtr createHullShader(ShaderDesc& desc) = 0;
	virtual ShaderPtr createDomainShader(ShaderDesc& desc) = 0;
	virtual ShaderPtr createGeometryShader(ShaderDesc& desc) = 0;
	virtual ShaderPtr createFragmentShader(ShaderDesc& desc) = 0;
	virtual ShaderPtr createComputeShader(ShaderDesc& desc) = 0;

	virtual TexturePtr create1DTexture(const TextureDesc& desc) = 0;
	virtual TexturePtr create2DTexture(const TextureDesc& desc) = 0;
	virtual TexturePtr create3DTexture(const TextureDesc& desc) = 0;

	virtual TexturePtr createRenderTarget(const TextureDesc& desc) = 0;
	virtual TexturePtr createDepthStencil(const TextureDesc& desc) = 0;

	virtual InputLayoutPtr createInputLayout(const VertexInputLayoutDesc& desc, const ShaderDesc& binary) = 0;

	virtual GpuBufferPtr createUniformBuffer(size_t size, const void* data) = 0;
	virtual GpuBufferPtr createIndexBuffer(EBufferUsage usage, size_t size, const void* data) = 0;
	virtual GpuBufferPtr createVertexBuffer(EBufferUsage usage, size_t size, const void* data) = 0;

	virtual CommandBufferPtr getActiveCommandBuffer() = 0;

	SamplerPtr createSampler(const SamplerDesc& desc)
	{
		auto sampler = m_SamplerFty.get(desc);
		if (!sampler)
		{
			sampler = createSampler_native(desc);
			m_SamplerFty.append(desc, sampler);
		}
		return sampler;
	}

	void clearResourceFactory()
	{
		m_SamplerFty.freeAll();
	}
protected:
	static const char8_t* const getVendorName(uint32_t verdorID)
	{
		switch (verdorID)
		{
		case 0x1002: return "AMD";
		case 0x1010: return "ImgTec";
		case 0x10DE: return "NVIDIA";
		case 0x13B5: return "ARM";
		case 0x5143: return "Qualcomm";
		case 0x8086: return "Intel";
		}

		return "Unknown";
	}

	virtual SamplerPtr createSampler_native(const SamplerDesc& desc) = 0;

	class SamplerFactory : public GfxResourceFactory<Sampler, SamplerDesc>
	{
	};

	SamplerFactory m_SamplerFty;
};

NAMESPACE_END(Gfx)
