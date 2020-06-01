#include "Colorful/Public/GfxModel.h"
#include "AssetTool/AssetDatabase.h"

void GfxModel::load(const std::string& modelName)
{
	if (!AssetTool::AssetDatabase::instance().tryToLoadStaticMesh(modelName, *this))
	{

	}
}
