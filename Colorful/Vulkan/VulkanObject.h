#pragma once

#include "VulkanTypes.h"

template <typename T> class VulkanObject
{
public:
	VulkanObject() = default;
	virtual ~VulkanObject() = default;

	inline T Get() const
	{
		assert(IsValid());
		return m_Handle;
	}

	inline VulkanObject(const VulkanObject &other)
	{
		assert(!IsValid());
		m_Handle = other.m_Handle;
	}

	inline VulkanObject(const T other)
	{
		assert(!IsValid());
		m_Handle = other;
	}

	inline void operator=(const VulkanObject &other)
	{
		///assert(!IsValid());
		m_Handle = other.m_Handle;
	}

	inline void operator=(const T other)
	{
		assert(!IsValid());
		m_Handle = other;
	}

	inline T *operator&()
	{
		return &m_Handle;
	}

	inline const T *operator&() const
	{
		assert(IsValid());
		return &m_Handle;
	}

	inline bool IsValid() const
	{
		return m_Handle != VK_NULL_HANDLE;
	}

	inline bool operator==(const VulkanObject &other) const
	{
		return m_Handle == other.m_Handle;
	}

	inline bool operator==(const T other) const
	{
		return m_Handle == other;
	}

	inline bool operator!=(const VulkanObject &other) const
	{
		return this->m_Handle != other.m_Handle;
	}

	inline bool operator!=(const T other) const
	{
		return m_Handle != other;
	}

	inline void Reset(const T other = VK_NULL_HANDLE)
	{
		m_Handle = other;
	}

	virtual void Destory() = 0;
protected:
	T m_Handle = VK_NULL_HANDLE;
private:
};

class VulkanViewport : public VkViewport
{
public:
	inline VulkanViewport() = default;

	inline VulkanViewport(
		float left,
		float top,
		float w,
		float h,
		float depthMin = 0.0f,
		float depthMax = 1.0f)
	{
		width = w;
		height = h;
		x = left;
		y = top;
		minDepth = depthMin;
		maxDepth = depthMax;
	}

	inline void SetSlot(uint32_t slot)
	{
		assert(slot != UINT32_MAX);
		m_Slot = slot;
	}

	inline uint32_t GetSlot() const
	{
		///assert(m_Slot != UINT32_MAX);
		return m_Slot;
	}
protected:
private:
	uint32_t m_Slot = UINT32_MAX;
};

class VulkanRect : public VkRect2D
{
public:
	inline VulkanRect() = default;
	inline VulkanRect(float32_t x, float32_t y, float32_t width, float32_t height)
	{
		offset.x = (int32_t)x;
		offset.y = (int32_t)y;
		extent.width = (uint32_t)width;
		extent.height = (uint32_t)height;
	}

	inline VulkanRect(const ::RECT &rect)
	{
		offset.x = (uint32_t)rect.left;
		offset.y = (uint32_t)rect.top;

		extent.width = (uint32_t)(rect.right - rect.left);
		extent.height = (uint32_t)(rect.bottom - rect.top);
	}

	inline void SetSlot(uint32_t slot)
	{
		assert(slot != UINT32_MAX);
		m_Slot = slot;
	}

	inline uint32_t GetSlot() const
	{
		///assert(m_Slot != UINT32_MAX);
		return m_Slot;
	}
protected:
private:
	uint32_t m_Slot = UINT32_MAX;
};

inline bool operator !=(const VulkanViewport &left, const VulkanViewport &right)
{
	return !Base::IsEqual(left, right);
}

inline bool operator !=(const VulkanRect &left, const VulkanRect &right)
{
	return !Base::IsEqual(left, right);
}