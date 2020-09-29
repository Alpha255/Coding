#pragma once

enum eRBufferUsage
{
	eGpuReadWrite,
	eGpuReadOnly,
	eGpuReadCpuWrite,
	eGpuCopyToCpu,
	eRBufferUsage_MaxEnum
};

enum eRBufferCpuAccessFlags
{
	eCpuRead,
	eCpuWrite,
	eCpuReadWrite,
	eRBufferCpuAccessFlags_MaxEnum
};

enum eRBufferBindFlags
{
	eVertexBuffer,
	eIndexBuffer,
	eUniformBuffer,
	eShaderResource,
	eStreamOutput,
	eRenderTarget,
	eDepthStencil,
	eUnorderedAccess,
	eDecoder,
	eVideoEncoder,
	eRBindFlags_MaxEnum
};

enum eRDepthStencilFlags
{
	eDepthReadOnly,
	eStencilReadOnly,
	eDepthStencilReadOnly,
	eRDepthStencilViewFlags_MaxEnum
};

enum eRClearFlags
{
	eClearDepth,
	eClearStencil,
	eClearDepthStencil,
	eRClearFlags_MaxEnum
};

enum eRVertexInputRate
{
	eVertex,
	eInstance,
	eRInputRate_MaxEnum
};

enum eRDescriptorType
{
	eDescriptor_Sampler,
	eDescriptor_CombinedTextureSampler,
	eDescriptor_SampledTexture,
	eDescriptor_StorageTexture,
	eDescriptor_UniformBuffer,
	eDescriptor_StorageBuffer,
	eDescriptor_InputAttachment,
	eRDescriptorType_MaxEnum
};
