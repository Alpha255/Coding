#pragma once

#include "Colorful/Gfx/GfxResource.h"

NAMESPACE_START(Gfx)

template <class T, typename ResourceDesc>
class GfxResourceFactory
{
public:
	using ResourcePtr = std::shared_ptr<T>;

	ResourcePtr get(const ResourceDesc& desc)
	{
		for (auto& pair : m_Resources)
		{
			if (pair.first == desc.hash())
			{
				return pair.second;
			}
		}

		return nullptr;
	}

	inline void append(const ResourceDesc& desc, ResourcePtr resource)
	{
		assert(resource);
		m_Resources.push_back(std::make_pair(desc.hash(), resource));
	}

	inline void freeAll()
	{
		m_Resources.clear();
	}
protected:
	std::vector<std::pair<uint64_t, ResourcePtr>> m_Resources; /// vector or map???
};

NAMESPACE_END(Gfx)
