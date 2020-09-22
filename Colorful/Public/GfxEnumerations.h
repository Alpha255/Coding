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

enum eRVertexUsage
{
	ePosition = 1 << 0,
	eNormal = 1 << 1,
	eTangent = 1 << 2,
	eBiNormal = 1 << 3,
	eBiTangent = 1 << 4,
	eTexcoord0 = 1 << 5,
	eTexcoord1 = 1 << 6,
	eTexcoord2 = 1 << 7,
	eTexcoord3 = 1 << 8,
	eTexcoord4 = 1 << 9,
	eTexcoord5 = 1 << 10,
	eTexcoord6 = 1 << 11,
	eTexcoord7 = 1 << 12,
	eColor0 = 1 << 13,
	eColor1 = 1 << 14,
	eColor2 = 1 << 15,
	eColor3 = 1 << 16,
	eWeight = 1 << 17,
	eRVertexUsge_MaxEnum = 18
};

enum class eRIndexType
{
	eUInt16,
	eUInt32,
	eRIndexType_MaxEnum
};
