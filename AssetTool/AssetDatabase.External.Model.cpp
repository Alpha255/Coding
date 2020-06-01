#include "AssetDatabase.h"
#include "Colorful/Public/GfxModel.h"
#include <ThirdParty/assimp/include/assimp/Importer.hpp>
#include <ThirdParty/assimp/include/assimp/scene.h>
#include <ThirdParty/assimp/include/assimp/postprocess.h>

namespaceStart(AssetTool)

AssetPtr AssetDatabase::tryToLoadStaticMesh(const std::string& modelName, GfxModel& gfxModel)
{
	auto model = AssetTool::AssetDatabase().instance().tryToGetAsset(modelName);
	assert(model->type() == AssetTool::Asset::eStaticMesh);

	///aiProcess_ConvertToLeftHanded;

	Assimp::Importer importer;
	auto scene = importer.ReadFile(model->fullPath(),
		(uint32_t)(
		aiPostProcessSteps::aiProcess_CalcTangentSpace |
		aiPostProcessSteps::aiProcess_Triangulate |
		aiPostProcessSteps::aiProcess_GenBoundingBoxes));
	if (!scene)
	{
		Logger::instance().log(Logger::eError, "Assimp: %s", importer.GetErrorString());
		return model;
	}

	if (!scene->HasMeshes())
	{
		Logger::instance().log(Logger::eError, "The model %s is empty", model->name().c_str());
		return nullptr;
	}

	for (uint32_t i = 0u; i < scene->mNumMeshes; ++i)
	{
		auto mesh = scene->mMeshes[i];
		assert(mesh);

		for (uint32_t j = 0u; j < mesh->mNumFaces; ++j)
		{
			auto& face = mesh->mFaces[j];
		}
	}

	return model;
}

namespaceEnd(AssetTool)