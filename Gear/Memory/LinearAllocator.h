#pragma once

#include "Gear/DesignPatterns/Singleton.h"

NAMESPACE_START(Gear)

/// Maybe a IAllocator

class LinearAllocator : public NoneCopyable
{
public:
	enum eAlignment
	{
		eDefaultAlignment = sizeof(uint32_t)
	};

	struct memoryPack
	{
		size_t Size = 0ull;
		byte8_t *Memory = nullptr;
	};

	explicit LinearAllocator(size_t size, size_t alignment = eDefaultAlignment)
	{
		size_t align = align_to(alignment, 2ull);
		m_Alignment = align < eDefaultAlignment ? eDefaultAlignment : align;
		assert(m_Current.is_lock_free());

		m_Size = align_to(size, m_Alignment);
		///m_Memory = std::make_unique<byte8_t>(new byte8_t[m_Size]());
		///assert(atomic_is_lock_free(m_Memory));
	}

	inline size_t totalSize() const
	{
		return m_Size;
	}

	inline size_t allocatedSize() const
	{
		return m_Current.load(std::memory_order_release);
	}

	inline size_t avaliableSize() const
	{
		return m_Size - allocatedSize();
	}

	inline size_t alignment() const
	{
		return m_Alignment;
	}

	static inline size_t align_to(size_t size, size_t alignment)
	{
		if (size % alignment == 0u)
		{
			return size;
		}

		return size + (alignment - size % alignment);
	}

	memoryPack alloc(size_t, size_t)
	{
		return memoryPack();
	}

	void dealloc(memoryPack &)
	{
	}

	void resize(size_t size)
	{
		if (size <= m_Size)
		{
			return;
		}

		//size_t newSize = align_to(size, m_Alignment);
		//std::unique_ptr<byte8_t> newMemory = std::make_unique<byte8_t>(new byte8_t[newSize]());
		//::memcpy_s(reinterpret_cast<void*>(*newMemory), m_Size, reinterpret_cast<const void*>(*m_Memory), m_Size);
		//m_Size = newSize;
		//m_Memory.reset(newMemory.release());
	}

private:
	size_t m_Size = 0u;
	size_t m_Alignment = 0u;
	std::unique_ptr<byte8_t> m_Memory = nullptr;
	std::atomic<size_t> m_Current = 0ull;
};

NAMESPACE_END(Gear)
