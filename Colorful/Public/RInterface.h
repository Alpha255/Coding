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

class rEngine
{
public:
	virtual void initialize(::HWND, const gear::appConfig &config) = 0;
	virtual void finalize() = 0;
	virtual void logError(uint32_t result) const = 0;

	virtual rDevicePtr getDevice() const = 0;
	virtual rContextPtr getIMContext() const = 0;
protected:
private:
};
typedef std::unique_ptr<rEngine> rEnginePtr;
extern rEnginePtr g_rEnginePtr;
