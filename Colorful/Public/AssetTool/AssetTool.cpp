#include "Colorful/Public/AssetTool/AssetTool.h"

NAMESPACE_START(Gfx)

static AssetToolPtr s_Instance = nullptr;
static std::string s_AssetDirectory;

AssetTool& AssetTool::instance()
{
	if (!s_Instance)
	{
		s_Instance = std::make_unique<AssetTool>();
	}

	return *s_Instance;
}

void AssetTool::initialize(const std::string& baseDirectory)
{
	assert(File::isExists(baseDirectory, true));

	s_AssetDirectory = File::directory(baseDirectory) + "\\Assets";
	assert(File::isExists(s_AssetDirectory, true));
	System::setWorkingDirectory(s_AssetDirectory);
	LOG_INFO("AssetDatabase:: Mount working directory to \"%s\"", System::getCurrentWorkingDirectory().c_str());
}

AssetPtr AssetTool::findAsset(const std::string& name)
{
	std::string path = File::find(s_AssetDirectory, name);
	if (File::isExists(path))
	{
		return std::make_shared<Asset>(path);
	}

	return AssetPtr();
}

std::vector<byte8_t> AssetTool::loadShader(Gfx::EShaderLanguage language, Gfx::EShaderStage stage, const std::string& name)
{
	std::vector<byte8_t> bytes;
	AssetPtr asset = findAsset(name);
	if (!asset)
	{
		return bytes;
	}



	return bytes;
}

NAMESPACE_END(Gfx)