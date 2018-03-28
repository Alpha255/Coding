#pragma once

#include "Common.h"

template <typename ResourceType> class D3DResource
{
public:
	D3DResource() = default;
	~D3DResource() = default;

	inline bool IsValid() const
	{
		return m_Resource.get() != nullptr;
	}

	inline const ResourceType *Get() const
	{
		return m_Resource.get();
	}

	inline D3DResource(const D3DResource &other)
	{
		assert(!IsValid());
		m_Resource = other.m_Resource;
	}

	inline void operator=(const D3DResource &other)
	{
		assert(!IsValid());
		m_Resource = other.m_Resource;
	}
protected:
	std::shared_ptr<ResourceType> m_Resource;
private:
};
