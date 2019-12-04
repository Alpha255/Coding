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

class rContext
{
};

class rTexture
{
};

class rBuffer
{
public:
	virtual void createAsIndexBuffer() = 0;
	virtual void createAsVertexBuffer() = 0;
};

class rShader
{
public:
	union rVertexLayout
	{

	};

	enum eVertexLayoutFlags
	{
		ePosition = 0x00000001,
		eNormal = 0x00000010,
		eTangent = 0x00000100,
		eTexcoord = 0x00001000,
	};

	rShader(eRShaderUsage usage)
		: m_Usage(usage)
	{
	}

	virtual void setVertexLayout(uint32_t vertexLayoutFlags) = 0;
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

class rDevice
{
public:
	virtual rShaderPtr createShader(eRShaderUsage usage, const std::string &shaderName) = 0;
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
