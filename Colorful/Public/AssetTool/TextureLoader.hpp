#include "Colorful/D3D/DXGI_Interface.h"
#include <ThirdParty/stb/stb_image.h>

NAMESPACE_START(Gfx)

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
	static TextureDesc load(const std::string& path)
	{
		auto format = getFormat(path);
		assert(format != ETextureFormat::Unsupported);

		if (format == ETextureFormat::DDS)
		{
			return load_DDS(path);
		}
		else if (format == ETextureFormat::KTX)
		{
			return load_KTX(path);
		}

		return load_General(path);
	}

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
protected:
	static TextureDesc load_General(const std::string& path)
	{
		int32_t width = 0u;
		int32_t height = 0u;
		int32_t channels = 0u;
		auto pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		return TextureDesc();
	}

	static TextureDesc load_DDS(const std::string& path)
	{
		return TextureDesc();
	}

	static TextureDesc load_KTX(const std::string& path)
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