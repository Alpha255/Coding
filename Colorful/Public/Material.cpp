#include "Colorful/Public/Material.h"
#include "AssetTool/AssetDatabase.h"
#include <iomanip>

Material::Material(const std::string& name)
{
	auto asset = AssetTool::AssetDatabase::instance().tryToGetAsset(name);
	assert(asset);

	auto file = std::static_pointer_cast<File>(asset);
	new (this)File(std::move(*file));
}

void Material::reload()
{
}
