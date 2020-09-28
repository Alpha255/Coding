#include "Colorful/Public/Material.h"
#include "AssetTool/AssetTool.h"
#include "Gear/IO/IO.h"

NAMESPACE_START(Gfx)

Material::Material(const std::string& name)
{
	auto asset = AssetTool::instance().findAsset(name);
	assert(asset);

	deserialize(asset->fullPath());
}

void Material::reload()
{
}

void Material::deserialize(const std::string& path)
{
	SyncContinuousIFStream is(path, File::EMode::Binary);

	uint32_t nameLength = is.read<uint32_t>();
	if (nameLength > 0u)
	{
		auto bytes = is.readBytes(nameLength);
		m_Name = std::string(reinterpret_cast<const char8_t*>(bytes.get()));
	}
	else
	{
		m_Name = "Unknown Material";
	}
}

NAMESPACE_END(Gfx)

