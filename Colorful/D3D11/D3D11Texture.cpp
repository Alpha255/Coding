#include "D3D11Texture.h"
#include "D3D11Engine.h"

void D3D11Texture1D::Create(
	uint32_t fmt,
	uint32_t width,
	uint32_t bindFlags,
	uint32_t mipLevels,
	uint32_t arraySize,
	uint32_t cpuFlags,
	uint32_t miscFlags,
	uint32_t usage,
	const D3D11SubResourceData *pSubResourceData)
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

	/// D3D11_SUBRESOURCE_DATA
	/// The distance (in bytes) from the beginning of one line of a texture to the next line. System-memory pitch is used only for 2D and 3D texture data as it is has no meaning for the other resource types.
	/// System-memory-slice pitch is only used for 3D texture data as it has no meaning for the other resource types

	ID3D11Texture1D *pTexture1D = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateTexture1D(&desc, pSubResourceData, &pTexture1D));

	Reset(pTexture1D);
}

void D3D11Texture1D::CreateRandomTexture()
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

	D3D11SubResourceData subResourceData
	{
		&random,
		size * sizeof(Vec4),
		0U
	};
	Create(eRGBA32_Float, size, eBindAsShaderResource, 1U, 1U, 0U, 0U,
		eGpuReadOnly, &subResourceData);

	///CreateShaderResourceView(rSRV, tex.Ptr(), DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_SRV_DIMENSION_TEXTURE1D);
}

void D3D11Texture2D::Create(
	uint32_t fmt,
	uint32_t width,
	uint32_t height,
	uint32_t bindFlags,
	uint32_t mipLevels,
	uint32_t arraySize,
	uint32_t cpuFlags,
	uint32_t miscFlags,
	uint32_t usage,
	const D3D11SubResourceData *pSubResourceData)
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

	ID3D11Texture2D *pTexture2D = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateTexture2D(&desc, pSubResourceData, &pTexture2D));

	Reset(pTexture2D);
}

void D3D11Texture3D::Create(
	uint32_t fmt,
	uint32_t width,
	uint32_t height,
	uint32_t depth,
	uint32_t bindFlags,
	uint32_t mipLevels,
	uint32_t cpuFlags,
	uint32_t miscFlags,
	uint32_t usage,
	const D3D11SubResourceData *pSubResourceData)
{
	assert(!IsValid());

	D3D11_TEXTURE3D_DESC desc = {};
	desc.Format = (DXGI_FORMAT)fmt;
	desc.Width = width;
	desc.Height = height;
	desc.Depth = depth;
	desc.BindFlags = bindFlags;
	desc.MipLevels = mipLevels;
	desc.CPUAccessFlags = cpuFlags;
	desc.MiscFlags = miscFlags;
	desc.Usage = (D3D11_USAGE)usage;

	ID3D11Texture3D *pTexture3D = nullptr;
	Check(D3D11Engine::Instance().GetDevice()->CreateTexture3D(&desc, pSubResourceData, &pTexture3D));

	Reset(pTexture3D);
}
