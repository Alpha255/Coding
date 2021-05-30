#include "Colorful/D3D/D3D11/D3D11Buffer.h"
#include "Colorful/D3D/D3D11/D3D11EnumTranslator.h"

NAMESPACE_START(Gfx)

ID3D11DeviceContext* D3D11Buffer::s_IMContext = nullptr;

D3D11Buffer::D3D11Buffer(
	ID3D11Device* device,
	ID3D11DeviceContext* IMContext,
	uint32_t bindFlags,
	EBufferUsage usage,
	size_t size,
	const void* data)
	: GpuBuffer(bindFlags & EBindFlags::UniformBuffer ? EDescriptorType::UniformBuffer : EDescriptorType::Null)
{
	/// https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-subresources

	assert(device && IMContext);

	if (!s_IMContext)
	{
		s_IMContext = IMContext;
	}

	m_Size = size;
	D3D11_BUFFER_DESC desc
	{
		static_cast<uint32_t>(size),
		D3D11EnumTranslator::usage(usage),
		D3D11EnumTranslator::bindFlags(bindFlags),
		CPUAccessFlags(usage),
		0u,
		0u
	};

	D3D11_SUBRESOURCE_DATA subresource
	{
		data,
		0u,
		0u
	};

	VERIFY_D3D(device->CreateBuffer(&desc, ((nullptr == data) ? nullptr : &subresource), reference()));
}

void* D3D11Buffer::map_native(size_t size, size_t offset)
{
	assert(isValid() && offset < m_Size && (~size == 0ULL || offset + size <= m_Size));

	if (!m_MappedMemory.Memory)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		VERIFY_D3D(s_IMContext->Map(get(), 0u, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0u, &mappedSubresource));
		m_MappedMemory.Memory = mappedSubresource.pData;
		m_MappedMemory.Size = size;
		m_MappedMemory.Offset = offset;
	}

	return m_MappedMemory.Memory;
}

void D3D11Buffer::update_native(const void* data, size_t size, size_t offset, bool8_t persistence)
{
	assert(isValid() && offset < m_Size && (~size == 0ULL || offset + size <= m_Size));

	map(size, offset);
	VERIFY(memcpy_s(m_MappedMemory.Memory, size, data, size) == 0);
	if (persistence)
	{
		flushMappedRange(size, offset);
	}
	else
	{
		unmap();
	}
}

void D3D11Buffer::flushMappedRange_native(size_t size, size_t offset)
{
	assert(0);
	(void)size;
	(void)offset;
}

void D3D11Buffer::unmap()
{
	assert(isValid());

	if (m_MappedMemory.Memory)
	{
		s_IMContext->Unmap(get(), 0u);
		m_MappedMemory.Memory = nullptr;
		m_MappedMemory.Size = 0u;
		m_MappedMemory.Offset = 0u;
	}
}

NAMESPACE_END(Gfx)