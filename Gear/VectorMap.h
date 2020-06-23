#pragma once

#include "Gear/Definitions.h"

namespaceStart(Gear)

template<class TKey, class TValue>
class VectorMap
{
public:
	struct KeyValue
	{
		TKey Key;
		TValue Value;
	};

	const KeyValue* const find(const TKey& key) const
	{
		for (uint32_t i = 0u; i < m_Pairs.size(); ++i)
		{
			if (m_Pairs[i].Key == key)
			{
				return &m_Pairs[i];
			}
		}

		return nullptr;
	}

	void insert(const TKey& key, const TValue& value)
	{
		if (!find(key))
		{
			m_Pairs.emplace_back(std::make_pair(key, value));
		}
	}
protected:
private:
	std::vector<KeyValue> m_Pairs;
};

namespaceEnd(Gear)
