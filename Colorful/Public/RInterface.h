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
};

class rBuffer
{
};

class rInputLayout
{

};

class rShader
{
public:
	rShader(eRShaderUsage usage)
		: m_Usage(usage)
	{
	}
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

class rRenderPass
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
	virtual void bindGraphicsPipeline() = 0;
protected:
private:
	std::string m_Description;
};

class rGraphicsPipeline
{
public:
	virtual void setInputLayout(const rInputLayout &inputLayout) = 0;

	virtual void setShaders(const rShader &shader) = 0;

	inline void setPrimitiveTopology(eRPrimitiveTopology primitiveTopology)
	{
		m_PrimitiveTopology = primitiveTopology;
	}

	virtual void setRasterizerState() = 0;

	virtual void setBlendState() = 0;

	virtual void setDepthStencilState() = 0;

	virtual void build() = 0;
protected:
	eRPrimitiveTopology m_PrimitiveTopology = eTriangleList;
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
	virtual void initialize(uint64_t windowHandl, const appConfig &config) = 0;
	virtual void finalize() = 0;
	virtual void logError(uint32_t result) const = 0;

	virtual void handleWindowResize(uint32_t width, uint32_t height, const appConfig &config) = 0;

	virtual rDevice *getDevice() = 0;
protected:
private:
};
