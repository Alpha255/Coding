#pragma once

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
