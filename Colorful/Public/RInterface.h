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

class rTexture
{
public:
	virtual void bindSampler(const class rSamplerState *samplerState) 
	{
		if (samplerState)
		{
			m_Sampler = samplerState;
		}
	}
protected:
	const class rSamplerState *m_Sampler = nullptr;
};

class rBuffer
{
};

class rInputLayout
{
public:
	struct rVertexAttributes
	{
		eRVertexUsage Usage = eRVertexUsge_MaxEnum;
		eRFormat Format = eRFormat_MaxEnum;
	};

	virtual void create(const std::vector<rVertexAttributes> &vertexAttributes) = 0;
};

class rShader
{
public:
	rShader(eRShaderUsage usage)
		: m_Usage(usage)
	{
	}

	virtual void setInputLayout(const rInputLayout *) {}
	virtual void pushUniformBuffer(const rBuffer *buffer) = 0;
	virtual void pushTexture(const rTexture *texture) = 0;
	virtual void pushSampler(const class rSamplerState *samplerState) = 0;
protected:
private:
	eRShaderUsage m_Usage = eRShaderUsage_MaxEnum;
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

class rDepthStencilState
{
};

class rBlendState
{
};

class rRasterizerState
{
};

class rSamplerState
{
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

	virtual void setInputLayout(const rInputLayout *inputLayout) = 0;

	virtual void setShader(const rShader *shader) = 0;

	virtual void setRasterizerState(const rRasterizerState *rasterizerState) = 0;

	virtual void setBlendState(const rBlendState *blendState) = 0;

	virtual void setDepthStencilState(const rDepthStencilState *depthStencilState) = 0;

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
private:
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

	virtual rDevice *getDevice() = 0;
protected:
private:
};
