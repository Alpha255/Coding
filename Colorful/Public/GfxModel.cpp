#include "Colorful/Public/GfxModel.h"
#include "AssetTool/AssetDatabase.h"

void GfxModel::load(const std::string& modelName, GfxEngine* gfxEngine)
{
	if (!AssetTool::AssetDatabase::instance().tryToLoadModel(modelName, *this, gfxEngine))
	{

	}
}
