#pragma once

#include "gear/gear.h"

class rObject
{
public:
	rObject() = default;
	virtual ~rObject() = default;

	virtual bool8_t isValid() = 0;
};

template <typename T> class vkObject : public rObject
{
public:
	inline bool8_t isValid() final
	{
		return m_Object != nullptr;
	}

	inline T get() const
	{
		return m_Object;
	}

	inline T *operator&()
	{
		return &m_Object;
	}

	inline void reset(const T other = nullptr)
	{
		m_Object = other;
	}
protected:
private:
	T m_Object = nullptr;
};

template <typename T> class d3dObject : public rObject
{
public:
	inline bool8_t isValid() final
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

	inline void reset(T *other = nullptr)
	{
		if (other)
		{
			m_Object.reset(other, std::function<void(T *&)>([](T *&object) 
			{ 
				object->Release();
				object = nullptr;
			}));
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

	inline uint32_t getRefCount() const
	{
		return m_Object.use_count();
	}
protected:
private:
	std::shared_ptr<T> m_Object = nullptr;
};
