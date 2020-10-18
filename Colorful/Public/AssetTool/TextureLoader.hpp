#include "Colorful/D3D/DXGI_Interface.h"
#include <ThirdParty/stb/stb_image.h>
#include <ThirdParty/DirectXTex/DirectXTex/DDS.h>

NAMESPACE_START(Gfx)

#if !defined(MAKEFOURCC)
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif

enum class ETextureFormat : uint8_t
{
	Unsupported,
	DDS,
	KTX,
	PNG,
	JPEG,
	BMP,
	TGA,
	PSD,
	GIF,
	HDR,
	PIC,
	PNM
};

struct TextureFormatAttribute
{
	ETextureFormat Format = ETextureFormat::Unsupported;
	std::string Extension;
	std::string Description;
};

class TextureLoader
{
public:
	static TextureDesc load(AssetPtr asset)
	{
		assert(asset);
		std::string path(std::move(asset->fullPath()));
		auto format = getFormat(asset->fullPath());
		assert(format != ETextureFormat::Unsupported);

		const byte8_t* data = asset->data().get();
		const size_t dataSize = asset->size();

		if (format == ETextureFormat::DDS)
		{
			return load_DDS(data, dataSize);
		}
		else if (format == ETextureFormat::KTX)
		{
			return load_KTX(data, dataSize);
		}

		return load_General(path);
	}
protected:
	static ETextureFormat getFormat(const std::string& path)
	{
		std::string extension(std::move(File::extension(path, true)));
		for each (auto& attr in s_SupportedFormats)
		{
			if (attr.Extension == extension)
			{
				return attr.Format;
			}
		}
		return ETextureFormat::Unsupported;
	}

	static TextureDesc load_General(const std::string& path)
	{
		int32_t width = 0u;
		int32_t height = 0u;
		int32_t channels = 0u;
		auto pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		return TextureDesc();
	}

	static TextureDesc load_DDS(const byte8_t* data, size_t size)
	{
		assert(data && size && size < std::numeric_limits<uint32_t>().max());
		assert(size >= (sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER)));
		assert(*reinterpret_cast<const uint32_t*>(data) == DirectX::DDS_MAGIC);

		bool8_t dxt10Header = false;
		auto header = reinterpret_cast<const DirectX::DDS_HEADER*>(data + sizeof(uint32_t));
		assert(header->size == sizeof(DirectX::DDS_HEADER) && header->ddspf.size == sizeof(DirectX::DDS_PIXELFORMAT));
		if ((header->ddspf.flags & DDS_FOURCC) && (header->ddspf.fourCC == MAKEFOURCC('D', 'X', '1', '0')))
		{
			assert(size >= (sizeof(DirectX::DDS_HEADER) + sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER_DXT10)));
			dxt10Header = true;
		}

		auto offset = sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER) + (dxt10Header ? sizeof(DirectX::DDS_HEADER_DXT10) : 0u);
		auto bitSize = size - offset;
		auto bitData = data + offset;

		TextureDesc desc{};
		desc.Width = header->width;
		desc.Height = header->height;
		desc.Depth = header->depth;
		desc.MipLevels = header->mipMapCount == 0u ? 1u : header->mipMapCount;

		if (dxt10Header)
		{
			auto dxtHeader = reinterpret_cast<const DirectX::DDS_HEADER_DXT10*>(reinterpret_cast<const uint8_t*>(header) + sizeof(DirectX::DDS_HEADER));
			assert(dxtHeader && dxtHeader->arraySize);

			desc.ArraySize = dxtHeader->arraySize;
		}
		else
		{

		}

		return TextureDesc();
	}

	static TextureDesc load_KTX(const byte8_t* data, size_t size)
	{
		return TextureDesc();
	}

private:
	static std::vector<TextureFormatAttribute> s_SupportedFormats;
};

std::vector<TextureFormatAttribute> TextureLoader::s_SupportedFormats =
{
	{ ETextureFormat::DDS,  ".dds", "DirectDraw Surface" },
	{ ETextureFormat::KTX,  ".ktx", "Khronos Texture" },
	{ ETextureFormat::PNG,  ".png", "Portable Network Graphics" },
	{ ETextureFormat::JPEG, ".jpg", "Joint Photographic Experts Group" },
	{ ETextureFormat::BMP,  ".bmp", "Bitmap" },
	{ ETextureFormat::TGA,  ".tga", "Truevision Graphics Adapter" },
	{ ETextureFormat::PSD,  ".psd", "Photoshop Document" },
	{ ETextureFormat::GIF,  ".gif", "Graphics Interchange Format" },
	{ ETextureFormat::HDR,  ".hdr", "High Dynamic Range" },
	{ ETextureFormat::PIC,  ".pic", "QuickTime Picture?" },
	{ ETextureFormat::PNM,  ".pgm", "Portable Anymap Format" }  /// Or .ppm
};

NAMESPACE_END(Gfx)