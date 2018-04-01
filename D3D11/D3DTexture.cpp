#include "D3DTexture.h"
#include "D3DEngine.h"
#include "System.h"
#include "D3DMath.h"

void D3DTexture1D::Create(
	uint32_t fmt,
	uint32_t width,
	uint32_t bindFlags,
	uint32_t mipLevels,
	uint32_t arraySize,
	uint32_t cpuFlags,
	uint32_t miscFlags,
	uint32_t usage,
	const void *pData,
	uint32_t memPitch)
{
	assert(!IsValid());

	D3D11_TEXTURE1D_DESC desc = {};
	desc.Format = (DXGI_FORMAT)fmt;
	desc.Width = width;
	desc.BindFlags = bindFlags;
	desc.MipLevels = mipLevels;
	desc.ArraySize = arraySize;
	desc.CPUAccessFlags = cpuFlags;
	desc.MiscFlags = miscFlags;
	desc.Usage = (D3D11_USAGE)usage;

	D3D11_SUBRESOURCE_DATA subResData;
	memset(&subResData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	subResData.pSysMem = pData;
	subResData.SysMemPitch = memPitch;
	subResData.SysMemSlicePitch = 0U; 	/// System-memory-slice pitch is only used for 3D texture data as it has no meaning for the other resource types

	ID3D11Texture1D *pTexture1D = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateTexture1D(&desc, ((nullptr == pData) ? nullptr : &subResData), &pTexture1D));

	MakeObject(pTexture1D);
}

void D3DTexture1D::CreateRandomTexture()
{
	assert(!IsValid());

	const uint32_t size = 1024U;
	Vec4 random[size] = {};

	for (uint32_t i = 0U; i < size; ++i)
	{
		random[i].x = Math::RandF(-1.0f, 1.0f);
		random[i].y = Math::RandF(-1.0f, 1.0f);
		random[i].z = Math::RandF(-1.0f, 1.0f);
		random[i].w = Math::RandF(-1.0f, 1.0f);
	}

	Create(DXGI_FORMAT_R32G32B32A32_FLOAT, size, D3D11_BIND_SHADER_RESOURCE, 1U, 1U, 0U, 0U,
		D3D11_USAGE_IMMUTABLE, random, size * sizeof(Vec4));

	///CreateShaderResourceView(rSRV, tex.Ptr(), DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_SRV_DIMENSION_TEXTURE1D);
}

void D3DTexture2D::Create(
	uint32_t fmt,
	uint32_t width,
	uint32_t height,
	uint32_t bindFlags,
	uint32_t mipLevels,
	uint32_t arraySize,
	uint32_t cpuFlags,
	uint32_t miscFlags,
	uint32_t usage,
	const void *pData,
	uint32_t memPitch)
{
	assert(!IsValid());

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Format = (DXGI_FORMAT)fmt;
	desc.Width = width;
	desc.Height = height;
	desc.BindFlags = bindFlags;
	desc.MipLevels = mipLevels;
	desc.ArraySize = arraySize;
	desc.CPUAccessFlags = cpuFlags;
	desc.SampleDesc.Count = 1U;
	desc.SampleDesc.Quality = 0U;
	desc.MiscFlags = miscFlags;
	desc.Usage = (D3D11_USAGE)usage;

	D3D11_SUBRESOURCE_DATA subResData = {};
	subResData.pSysMem = pData;
	subResData.SysMemPitch = memPitch;
	subResData.SysMemSlicePitch = 0U; 	/// System-memory-slice pitch is only used for 3D texture data as it has no meaning for the other resource types

	ID3D11Texture2D *pTexture2D = nullptr;
	HRCheck(D3DEngine::Instance().GetDevice()->CreateTexture2D(&desc, ((nullptr == pData) ? nullptr : &subResData), &pTexture2D));

	MakeObject(pTexture2D);
}
