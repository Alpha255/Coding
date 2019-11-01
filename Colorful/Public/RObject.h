#pragma once

#include "gear/gear.h"

template <typename T> class rObject
{
public:
	inline rObject() = default;
	virtual ~rObject() = default;

	inline bool8_t isValid() const
	{
		return m_Object != nullptr;
	}

	inline T *get() const
	{
		return m_Object.get();
	}

	inline T *operator->() const
	{
		assert(isValid());
		return get();
	}

	inline uint32_t getRefCount() const
	{
		return m_Object.use_count();
	}

	inline void reset(T *other = nullptr)
	{
		if (other)
		{
			destory();
			m_Object.reset(other);
		}
		else
		{
			if (m_Object)
			{
				m_Object.reset();
				m_Object = nullptr;
			}
		}
	}

	inline virtual void destory() {};
protected:
	std::shared_ptr<T> m_Object = nullptr;
private:
};

template <typename T> class vkObject : public rObject<T>
{
public:
};

template <typename T> class d3dObject : public rObject<T>
{
public:
	inline void destory() override final
	{
		if (m_Object)
		{
			m_Object->Release();
			m_Object = nullptr;
		}
	}
};

#define rVerify(func)     \
{                         \
	auto result = (func); \
	if (result != 0)      \
	{                     \
	}                     \
}
