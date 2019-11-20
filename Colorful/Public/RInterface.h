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

class rDevice
{
protected:
	rAdapter m_Adapter;
};

class rContext
{
};

class rSwapchain
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
	class rShaderInput
	{
		class rSampler
		{
		};
	};
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

typedef std::shared_ptr<rDevice> rDevicePtr;
typedef std::shared_ptr<rContext> rContextPtr;
typedef std::shared_ptr<rTexture> rTexturePtr;

class rEngine
{
public:
	virtual void initialize(uint64_t windowHandl, const appConfig &config) = 0;
	virtual void finalize() = 0;
	virtual void logError(uint32_t result) const = 0;

	virtual rDevicePtr getDevice() const = 0;
	virtual rContextPtr getIMContext() const = 0;
protected:
private:
};
typedef std::unique_ptr<rEngine> rEnginePtr;
extern rEnginePtr g_rEnginePtr;
