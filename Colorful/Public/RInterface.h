#pragma once

#include "RObject.h"

namespace gear
{
	struct appConfig;
}

struct rAdapter
{
	std::string DeviceName;
	uint32_t VendorID = 0u;
	uint32_t DeviceID = 0u;                      

	static std::string getVerdorName(uint32_t verdorID)
	{
		std::string verdorName("Unknown");
		switch (verdorID)
		{
		case 0x1002: 
			verdorName = std::string("AMD");
			break;
		case 0x1010: 
			verdorName = std::string("ImgTec");
			break;
		case 0x10DE: 
			verdorName = std::string("NVIDIA");
			break;
		case 0x13B5: 
			verdorName = std::string("ARM");
			break;
		case 0x5143: 
			verdorName = std::string("Qualcomm");
			break;
		case 0x8086: 
			verdorName = std::string("Intel");
			break;
		}

		return verdorName;
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
	class rShaderInput
	{
		class rSampler
		{
		};
	};

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

class rGpuMarker
{
};

class rGpuTimer
{
};

class rResourceFactory
{
public:
	enum eResourceType
	{
		eInputLayout,
		eBuffer,
	};

	struct rCreateInfo
	{
	};
};

class rInputLayout
{

};

class rDevice
{
public:
	virtual rShader *createShader(eRShaderUsage usage, const std::string &shaderName) = 0;
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


extern rEngine *g_rEnginePtr;
extern rDevice *g_rDevicePtr;
