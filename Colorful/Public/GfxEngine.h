#pragma once

#include "GfxDefinitions.h"
#include "GfxRenderState.h"

namespace Gear
{
	struct Configurations;
}

struct GfxAdapter
{
	std::string DeviceName;
	uint32_t VendorID = 0u;
	uint32_t DeviceID = 0u;                      

	static std::string vendorName(uint32_t verdorID)
	{
		std::string name("Unknown");
		switch (verdorID)
		{
		case 0x1002: 
			name = std::string("AMD");
			break;
		case 0x1010: 
			name = std::string("ImgTec");
			break;
		case 0x10DE: 
			name = std::string("NVIDIA");
			break;
		case 0x13B5: 
			name = std::string("ARM");
			break;
		case 0x5143: 
			name = std::string("Qualcomm");
			break;
		case 0x8086: 
			name = std::string("Intel");
			break;
		}

		return name;
	}
};

class GfxDevice
{
protected:
	GfxAdapter m_Adapter;
};

class rRenderSurface
{
};

class rGeometryBuffer
{
};

class rGpuMarker
{
};

class rGpuTimer
{
};

class GfxEngine
{
public:
	virtual void initialize(uint64_t windowHandle, const Gear::Configurations &config) = 0;
	virtual void finalize() = 0;
	virtual void logError(uint32_t result) const = 0;
	virtual void handleWindowResize(uint32_t width, uint32_t height, const Gear::Configurations &config) = 0;
	virtual void present() {}

	virtual class rShader *createVertexShader(const std::string &shaderName) = 0;
	virtual class rShader *createFragmentShader(const std::string &shaderName) = 0;

	virtual class rBuffer *createIndexBuffer(eRBufferUsage, size_t, const void *) { return nullptr; }
	virtual class rBuffer *createVertexBuffer(eRBufferUsage, size_t, const void *) { return nullptr; }
	virtual class rBuffer *createUniformBuffer(size_t, const void *) { return nullptr; }
	virtual void destroyBuffer(class rBuffer *) {}

	virtual rRenderSurface *createDepthStencilView(uint32_t, uint32_t, eRFormat) { return nullptr; }
	virtual GfxRenderPass *createRenderPass(GfxFrameBufferDesc &) { return nullptr; }
	virtual void createOpaqueRenderPass() {}

	virtual class rTexture *createTexture(const std::string &) { return nullptr; }
	virtual class rTexture *createTexture(
		eRTextureType,
		eRFormat,
		uint32_t,
		uint32_t,
		uint32_t,
		uint32_t,
		const void *,
		size_t) 
	{
		return nullptr;
	}

	virtual void updateGpuBuffer(class rBuffer *, const void *, size_t, size_t) {}

	virtual rSampler *createSampler(const GfxSamplerDesc &) { return nullptr; }

	inline GfxRenderPass *getOpaqueRenderPass()
	{
		assert(m_OpaqueRenderPass);
		return m_OpaqueRenderPass;
	}
protected:
	GfxRenderPass *m_OpaqueRenderPass = nullptr;
private:
};
