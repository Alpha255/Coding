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

class rGpuMarker
{
};

class rGpuTimer
{
};

class rDevice
{
public:
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
	virtual void present() {}

	virtual class rShader *createVertexShader(const std::string &shaderName) = 0;
	virtual class rShader *createFragmentShader(const std::string &shaderName) = 0;

	virtual class rBuffer *createIndexBuffer(eRBufferUsage, size_t, const void *) { return nullptr; }
	virtual class rBuffer *createVertexBuffer(eRBufferUsage, size_t, const void *) { return nullptr; }
	virtual class rBuffer *createUniformBuffer(size_t, const void *) { return nullptr; }

	virtual class rRenderSurface *createDepthStencilView(uint32_t, uint32_t, eRFormat) { return nullptr; }
	virtual class rRenderPass *createRenderPass(struct rFrameBufferDesc &) { return nullptr; }

	virtual class rTexture *createTexture(const std::string &) { return nullptr; }

	virtual void updateUniformBuffer(class rBuffer *, const void *, size_t, size_t) {}

	virtual class rSampler *createSampler(const struct rSamplerDesc &) { return nullptr; }
protected:
private:
};

#include "RRenderState.h"
