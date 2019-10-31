#pragma once

#include "gear/gear.h"

class rDevice
{
public:
	virtual void create(::HWND) = 0;
	virtual void destory() = 0;
	virtual uint32_t translate(uint32_t) = 0;
};

class rContext
{
};

class rObject
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
