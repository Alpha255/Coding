#include "Colorful/Public/GfxRenderState.h"

NAMESPACE_START(Gfx)

DECLARE_UNIQUE_PTR(GfxResourceManager)
class GfxResourceManager
{
public:
	virtual ~GfxResourceManager() = default;

	virtual ShaderPtr createShader(const ShaderDesc& desc) = 0;
	virtual TexturePtr createTexture(const TextureDesc& desc) = 0;
	virtual SamplerPtr createSampler(const SamplerDesc& desc) = 0;
	virtual InputLayoutPtr createInputLayout(const std::vector<VertexInputDesc>& descs, const std::vector<uint32_t>& binary) = 0;
	
	virtual GPUBufferPtr createUniformBuffer(size_t size, const void* data) = 0;
	virtual GPUBufferPtr createIndexBuffer(EBufferUsage usage, size_t size, const void* data) = 0;
	virtual GPUBufferPtr createVertexBuffer(EBufferUsage usage, size_t size, const void* data) = 0;
};

extern EXPORT_API GfxResourceManager* GResourceManager;

NAMESPACE_END(Gfx)