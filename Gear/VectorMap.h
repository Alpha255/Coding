#pragma once

#include "Gear/Definitions.h"

namespaceStart(Gear)

template<class TKey, class TValue>
class VectorMap
{
public:
	struct KeyValue
	{
		TKey first;
		TValue second;
	};

#if defined(_DEBUG)
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

	KeyValue* find(const TKey& key)
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

	KeyValue* operator[](const TKey& key)
	{
		for (uint32_t i = 0u; i < m_Pairs.size(); ++i)
		{
			if (m_Pairs[i].Key == key)
			{
				return &m_Pairs[i];
			}
		}

		TValue value;
		m_Pairs.emplace_back(std::make_pair(key, value));
		return nullptr;
	}

	const KeyValue* const operator[](const TKey& key) const
	{
		for (uint32_t i = 0u; i < m_Pairs.size(); ++i)
		{
			if (m_Pairs[i].Key == key)
			{
				return &m_Pairs[i];
			}
		}

		TValue value;
		m_Pairs.emplace_back(std::make_pair(key, value));
		return nullptr;
	}

	void insert(const TKey& key, const TValue& value)
	{
		if (!find(key))
		{
			m_Pairs.emplace_back(std::make_pair(key, value));
		}
	}
#else
	inline auto find(const TKey& key)
	{
		return m_Map.find(key);
	}

	inline auto operator[](const TKey& key)
	{
		return m_Map[key];
	}

	inline void insert(const TKey& key, const TValue& value)
	{
		m_Map.insert(std::make_pair(key, value));
	}
#endif
protected:
private:
#if defined(_DEBUG)
	std::vector<KeyValue> m_Pairs;
#else
	std::unordered_map<TKey, TValue> m_Map;
#endif
};

namespaceEnd(Gear)
