#pragma once

#include "RObject.h"

class rDevice
{
public:
	virtual void create() = 0;
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
	virtual void initialize(::HWND, uint32_t, uint32_t, bool8_t) = 0;

	inline rDevicePtr getDevice() const
	{
		assert(m_Device);
		return m_Device;
	}

	inline rContextPtr getIMContext() const
	{
		assert(m_IMContext);
		return m_IMContext;
	}
protected:
private:
	rDevicePtr m_Device = nullptr;
	rContextPtr m_IMContext = nullptr;
};
