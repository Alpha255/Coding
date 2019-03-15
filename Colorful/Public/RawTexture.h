#pragma once

#include "Common.h"
#include <ThirdParty/gli/gli/gli.hpp>
#include <ThirdParty/gli/gli/texture2d.hpp>

namespace DirectX
{
	struct DDS_PIXELFORMAT;
}

struct RawTexture
{
	struct RawMipTexture
	{
		const uint8_t *RawMipData = nullptr;
		uint32_t MipWidth = 0U;
		uint32_t MipHeight = 0U;
		size_t MipSize = 0U;
		size_t Offset = 0U;
	};

	uint32_t Width = 0U;
	uint32_t Height = 0U;
	uint32_t MipLevels = 0U;
	uint32_t Depth = 0U;
	uint32_t ArraySize = 0U;
	uint32_t Format = 0U;
	uint32_t Dimension = 0U;
	size_t RawSize = 0U;

	std::shared_ptr<uint8_t> RawData = nullptr;
	std::vector<RawMipTexture> RawMipTextures;

	bool CubeMap = false;
	bool Compressed = false;

	void CreateFromDds(const char *pFileName, bool bUsingVulkan);
protected:
	size_t GetBitsPerPixel(uint32_t format);
	uint32_t GetDXGIFormat(const DirectX::DDS_PIXELFORMAT &ddpf);
	void SetupMipMaps();
	void TranslateToVulkanDimension();
	void TranslateToVulkanFormat();

	gli::texture2d RawTex2D;
};
