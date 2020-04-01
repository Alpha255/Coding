#pragma once

#include "Gear/Public/Independent/Singleton.h"

namespaceStart(gear)

template<size_t Alignment> class linearAllocator : public noneCopyable
{
public:
	struct memoryPack
	{
		size_t Size = 0ull;
		byte8_t *Memory = nullptr;
	};

	explicit linearAllocator(size_t size)
		: linearAllocator()
		, m_Size(size)
	{
		m_Memory = std::make_unique<byte8_t>(new byte8_t[align_to(size, m_Alignment)]());
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

	static inline size_t align_to(size_t size, size_t alignment)
	{
		return size + size % alignment;
	}

	memoryPack alloc(size_t size, size_t alignment);

	void dealloc(memoryPack &pack);

	void resize(size_t size);

private:
	linearAllocator()
	{
		size_t alignment = align_to(Alignment, 2ull);
		m_Alignment = alignment < c_DefaultAlignment ? c_DefaultAlignment : alignment;
		assert(m_Current.is_lock_free());
	}

	size_t m_Size = 0u;
	size_t m_Alignment = 0u;
	std::unique_ptr<byte8_t> m_Memory = nullptr;
	std::atomic<size_t> m_Current = 0ull;
	constexpr size_t c_DefaultAlignment = sizeof(uint32_t);
};

template<size_t Alignment>
linearAllocator<Alignment>::memoryPack linearAllocator<Alignment>::alloc(size_t size, size_t alignment)
{
	return nullptr;
}

template<size_t Alignment>
void linearAllocator<Alignment>::dealloc(memoryPack &pack)
{
}

template<size_t Alignment>
void linearAllocator<Alignment>::resize(size_t size)
{
	if (size <= m_Size)
	{
		return;
	}

	size_t newSize = align_to(size, m_Alignment);
	std::unique_ptr<byte8_t> newMemory = std::make_unique<byte8_t>(new byte8_t[newSize]());
	::memcpy_s(*newMemory, m_Size, *m_Memory, m_Size);
	m_Size = newSize;
	m_Memory = newMemory.release();
}

namespaceEnd(gear)
