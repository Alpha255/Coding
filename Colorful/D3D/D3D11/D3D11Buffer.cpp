#include "Colorful/D3D/D3D11/D3D11Buffer.h"
#include "Colorful/D3D/D3D11/D3D11Map.h"

NAMESPACE_START(Gfx)

D3D11Buffer::D3D11Buffer(
	ID3D11Device* device,
	uint32_t bindFlags,
	EBufferUsage usage,
	size_t size,
	const void* data)
{
	assert(device);

	D3D11_BUFFER_DESC desc
	{
		static_cast<uint32_t>(size),
		D3D11Map::usage(usage),
		D3D11Map::bindFlags(bindFlags),
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

NAMESPACE_END(Gfx)
#if 0
#include "Colorful/D3D/D3D11/D3D11Engine.h"

void D3D11Buffer::update(const void* data, size_t size, size_t offset)
{
	assert(isValid());
	D3D11BufferPool::instance()->updateBuffer(this, data, size, offset);
}
#endif