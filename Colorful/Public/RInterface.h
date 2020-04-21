#pragma once

#include "RDeclaration.h"

namespace gear
{
	struct appConfig;
}

struct rAdapter
{
	std::string DeviceName;
	uint32_t VendorID = 0u;
	uint32_t DeviceID = 0u;                      

	static std::string getVendorName(uint32_t verdorID)
	{
		std::string vendorName("Unknown");
		switch (verdorID)
		{
		case 0x1002: 
			vendorName = std::string("AMD");
			break;
		case 0x1010: 
			vendorName = std::string("ImgTec");
			break;
		case 0x10DE: 
			vendorName = std::string("NVIDIA");
			break;
		case 0x13B5: 
			vendorName = std::string("ARM");
			break;
		case 0x5143: 
			vendorName = std::string("Qualcomm");
			break;
		case 0x8086: 
			vendorName = std::string("Intel");
			break;
		}

		return vendorName;
	}
};

class rGpuResource
{
public:
	enum eResourceType
	{
		eTexture,
		eShader,
		eBuffer,
		eResourceType_MaxEnum
	};
};

class rSampler
{
};

class rTexture : public rGpuResource
{
public:
	void bindSampler(const rSampler *sampler) 
	{
		m_Sampler = sampler;
	}
protected:
	const rSampler *m_Sampler = nullptr;
};

class rBuffer : public rGpuResource
{
};

struct rVertexAttributes
{
	eRVertexUsage Usage = eRVertexUsge_MaxEnum;
	eRFormat Format = eRFormat_MaxEnum;
	eRVertexInputRate InputRate = eRInputRate_MaxEnum;

	static uint32_t getUsageStride(eRVertexUsage usage);
	static size_t getFormatStride(eRFormat format);
};

struct rDescriptorLayoutDesc
{
	inline void pushDescriptorSet(eRShaderUsage shader, eRDescriptorType type)
	{
		assert(shader < eRShaderUsage_MaxEnum && type < eRDescriptorType_MaxEnum);
		DescriptorLayout[shader].emplace_back(type);
	}

	std::array<std::vector<eRDescriptorType>, eRShaderUsage_MaxEnum> DescriptorLayout;
};

class rShader : public rGpuResource
{
public:
	rShader(eRShaderUsage usage)
		: m_Usage(usage)
	{
	}

	inline eRShaderUsage getUsage() const
	{
		return m_Usage;
	}

	inline void bindTexture(const rTexture *texture)
	{
		assert(texture);
		m_Textures.emplace_back(std::move(texture));
	}
	inline void bindSampler(const rSampler *sampler)
	{
		assert(sampler);
		m_Samplers.emplace_back(std::move(sampler));
	}
	inline void bindUniformBuffer(const rBuffer *buffer)
	{
		assert(buffer && !m_UniformBuffer);
		m_UniformBuffer = buffer;
	}
protected:
	eRShaderUsage m_Usage = eRShaderUsage_MaxEnum;
	std::vector<const rTexture *> m_Textures;
	std::vector<const rSampler *> m_Samplers;
	const rBuffer *m_UniformBuffer = nullptr;
private:
};

class rDescriptor
{
public:
	virtual void bindVertexBuffer() = 0;
	virtual void bindIndexBuffer() = 0;
};

class rRenderpassDesc
{

};

class rRenderSurface
{
};

class rGeometryBuffer
{
};

class rCommandList
{
};

class rCommandBuffer
{
};

class rViewport
{
};

class rScissor
{
};

struct rStencilOp
{
	eRStencilOp FailOp = eRStencilOp::eRStencilOp_MaxEnum;
	eRStencilOp PassOp = eRStencilOp::eRStencilOp_MaxEnum;
	eRStencilOp DepthFailOp = eRStencilOp::eRStencilOp_MaxEnum;
	eRCompareOp CompareOp = eRCompareOp::eRCompareOp_MaxEnum;
};

struct rColorBlendState
{
	bool8_t Enable = false;
	eRBlendFactor SrcColor = eRBlendFactor::eRBlendFactor_MaxEnum;
	eRBlendFactor DstColor = eRBlendFactor::eRBlendFactor_MaxEnum;
	eRBlendOp ColorOp = eRBlendOp::eRBlendOp_MaxEnum;
	eRBlendFactor SrcAlpha = eRBlendFactor::eRBlendFactor_MaxEnum;
	eRBlendFactor DstAlpha = eRBlendFactor::eRBlendFactor_MaxEnum;
	eRBlendOp AlphaOp = eRBlendOp::eRBlendOp_MaxEnum;
	uint32_t ColorMask = eRColorWriteMask::eColorNone;
};

class rGpuMarker
{
};

class rGpuTimer
{
};

class rRenderpass
{
public:
	virtual void begin() = 0;
	virtual void end() = 0;
	virtual void execute(const class rGraphicsPipeline *pipline) = 0;
protected:
private:
	std::string m_Description;
};

class rGraphicsPipeline
{
public:
	inline void setPrimitiveTopology(eRPrimitiveTopology primitiveTopology)
	{
		m_PrimitiveTopology = primitiveTopology;
	}

	inline void setClearValue(const vec4 &color, const float32_t depth, const uint8_t stencil)
	{
		m_ClearValue.Color = color;
		m_ClearValue.Depth = depth;
		m_ClearValue.Stencil = stencil;
	}

	inline void setShader(const rShader *shader)
	{
		assert(shader);
		assert(shader->getUsage() < eRShaderUsage_MaxEnum);
		assert(m_Shaders[shader->getUsage()] == nullptr);
		m_Shaders[shader->getUsage()] = shader;
	}

	virtual void setRasterizerState(
		eRPolygonMode polygonMode,
		eRCullMode cullMode,
		eRFrontFace frontFace,
		bool8_t enableDepthBias = false,
		float32_t depthBias = 0.0f,
		float32_t depthBiasClamp = 0.0f,
		float32_t depthBiasSlope = 0.0f) = 0;

	virtual void setBlendState(
		bool8_t enableLogicOp,
		eRLogicOp logicOp,
		uint32_t attachmentCount,
		const rColorBlendState * const clrBlendState) = 0;

	virtual void setDepthStencilState(
		bool8_t enableDepth,
		bool8_t enableDepthWrite,
		eRCompareOp depthCompareOp,
		bool8_t enableStencil,
		uint8_t stencilReadMask,
		uint8_t stencilWriteMask,
		const rStencilOp &front,
		const rStencilOp &back) = 0;

	virtual void setDescriptorLayout(const rDescriptorLayoutDesc &desc) = 0;

	virtual void setViewport(const rViewport *viewport) = 0;

	virtual void setScissor(const rScissor *scissor) = 0;

	virtual void build(const rRenderpass &renderpass) = 0;
protected:
	eRPrimitiveTopology m_PrimitiveTopology = eTriangleList;
	struct ClearValue
	{
		vec4 Color;
		float32_t Depth = 1.0f;
		uint8_t Stencil = 0u;
	} m_ClearValue;

	const rShader *m_Shaders[eRShaderUsage_MaxEnum]{};
private:
};

class rComputePipeline
{
};

class rDevice
{
public:
	virtual rShaderPtr createShader(eRShaderUsage usage, const std::string &shaderName) = 0;
	virtual rTexturePtr createTexture(const std::string &textureName) = 0;
protected:
	rAdapter m_Adapter;
private:
};

class rEngine
{
public:
	virtual void initialize(uint64_t windowHandle, const appConfig &config) = 0;
	virtual void finalize() = 0;
	virtual void logError(uint32_t result) const = 0;

	virtual void handleWindowResize(uint32_t width, uint32_t height, const appConfig &config) = 0;
protected:
private:
};
