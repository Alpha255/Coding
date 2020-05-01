#pragma once

#include "RObject.h"

#if defined(max)
#undef max

struct rVertexAttributes
{
	eRVertexUsage Usage = eRVertexUsge_MaxEnum;
	eRFormat Format = eRFormat_MaxEnum;
	eRVertexInputRate InputRate = eRInputRate_MaxEnum;

	static uint32_t getUsageStride(eRVertexUsage usage);
	static size_t getFormatStride(eRFormat format);
};

class rGpuResource
{
public:
	inline uint64_t getID() const
	{
		return m_ID;
	}

	inline void setID(uint64_t id)
	{
		m_ID = id;
	}
protected:
	uint64_t m_ID = 0u;
private:
};

class rShader : public rGpuResource
{
public:
	struct rReflectionInfo
	{
		uint32_t Type = std::numeric_limits<uint32_t>::max();
		uint32_t Binding = 0u;
	};
	using rReflectionInfos = std::vector<rReflectionInfo>;

	rShader(eRShaderUsage usage)
		: m_Usage(usage)
	{
	}

	inline eRShaderUsage getUsage() const
	{
		return m_Usage;
	}

	inline const rReflectionInfos &getReflections() const
	{
		return m_Reflections;
	}

	inline void setUniformBuffer(class rBuffer *uniformBuffer)
	{
		m_UniformBuffer = uniformBuffer;
	}
	inline class rBuffer *getUniformBuffer() const
	{
		return m_UniformBuffer;
	}
	inline void bindTexture(const class rTexture *texture)
	{
		Textures.emplace_back(texture);
	}

	inline const std::vector<const class rTexture *> &getTextures() const
	{
		return Textures;
	}

	virtual void setInputLayout(const std::vector<rVertexAttributes>&, size_t) {}
protected:
	eRShaderUsage m_Usage = eRShaderUsage_MaxEnum;
	rReflectionInfos m_Reflections;
	class rBuffer *m_UniformBuffer = nullptr;
	std::vector<const class rTexture *> Textures;
private:
};

class rTexture : public rGpuResource
{
public:
	inline void bindSampler(const class rSampler *sampler)
	{
		m_Sampler = sampler;
	}

	inline const class rSampler *getSampler() const
	{
		return m_Sampler;
	}
protected:
	const class rSampler *m_Sampler = nullptr;
};

class rBuffer : public rGpuResource
{
public:
	inline size_t getSize() const
	{
		return m_Size;
	}

	inline size_t getOffset() const
	{
		return m_Offset;
	}
protected:
	size_t m_Size = 0u;
	size_t m_Offset = 0u;
private:
};

using rDescriptorLayoutDesc = std::array<rShader::rReflectionInfos, eRShaderUsage_MaxEnum>;

class rDescriptor
{
public:
	virtual void bindVertexBuffer() = 0;
	virtual void bindIndexBuffer() = 0;
};

#endif
