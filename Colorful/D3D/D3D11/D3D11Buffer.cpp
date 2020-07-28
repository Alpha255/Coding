#include "Colorful/D3D/D3D11/D3D11Engine.h"

D3D11Buffer::D3D11Buffer(
	const D3D11Device& device, 
	eRBufferBindFlags bindFlags, 
	eRBufferUsage usage, 
	size_t size, 
	const void* data)
{
	assert(device.isValid());

	D3D11_BUFFER_DESC desc =
	{
		(uint32_t)size,
		D3D11Enum::toUsage(usage),
		getBindFlags(bindFlags),
		getCpuAccessFlags(usage),
		0u,
		0u
	};

	D3D11_SUBRESOURCE_DATA subResourceDesc
	{
		data,
		0u,
		0u
	};

	ID3D11Buffer* buffer = nullptr;
	GfxVerifyD3D(device->CreateBuffer(&desc, ((nullptr == data) ? nullptr : &subResourceDesc), &buffer));
	reset(buffer);
}

void D3D11Buffer::update(const void* data, size_t size, size_t offset)
{
	assert(isValid());
	D3D11BufferPool::instance()->updateBuffer(this, data, size, offset);
}

void D3D11BufferPool::updateBuffer(D3D11BufferPtr buffer, const void* data, size_t size, size_t offset)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	GfxVerifyD3D(m_IMContext->Map(buffer->get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedSubresource));
	verify(memcpy_s(mappedSubresource.pData, size, ((byte8_t *)data + offset), size) == 0);
	m_IMContext->Unmap(buffer->get(), 0u);
}
