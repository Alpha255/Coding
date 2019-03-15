#include "D3D11Buffer.h"
#include "D3D11Engine.h"

void D3D11Buffer::Create(
	uint32_t bindFlags,
	size_t byteWidth,
	uint32_t usage,
	const void *pData,
	uint32_t cpuAccessFlag,
	uint32_t miscFlags,
	uint32_t byteStride,
	uint32_t memPitch)
{
	assert(!IsValid() && byteWidth <= UINT32_MAX);

	D3D11_BUFFER_DESC bufDesc = {};
	D3D11_SUBRESOURCE_DATA srcData = {};

	bufDesc.ByteWidth = (uint32_t)byteWidth;
	bufDesc.Usage = (D3D11_USAGE)usage;
	bufDesc.BindFlags = bindFlags;
	bufDesc.CPUAccessFlags = cpuAccessFlag;
	bufDesc.MiscFlags = miscFlags;
	bufDesc.StructureByteStride = byteStride;

	srcData.pSysMem = pData;
	srcData.SysMemPitch = memPitch;
	srcData.SysMemSlicePitch = 0U;

	ID3D11Buffer *pBuffer = nullptr;
	HRCheck(D3D11Engine::Instance().GetDevice()->CreateBuffer(&bufDesc, ((nullptr == pData) ? nullptr : &srcData), &pBuffer));

	MakeObject(pBuffer);
}

void D3D11Buffer::Update(const void *pData, size_t size)
{
	assert(pData && size && IsValid());

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRCheck(D3D11Engine::Instance().GetIMContext()->Map(Get(), 0U, D3D11_MAP_WRITE_DISCARD, 0U, &mapped)); 	/// Map cannot be called with MAP_WRITE_DISCARD access
	memcpy(mapped.pData, pData, size);
	D3D11Engine::Instance().GetIMContext()->Unmap(Get(), 0U);
}