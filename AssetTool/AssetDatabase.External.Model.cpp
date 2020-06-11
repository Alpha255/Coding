#include "AssetDatabase.h"
#include "Colorful/Public/GfxEngine.h"
#include <ThirdParty/assimp/include/assimp/Importer.hpp>
#include <ThirdParty/assimp/include/assimp/scene.h>
#include <ThirdParty/assimp/include/assimp/postprocess.h>

namespaceStart(AssetTool)

AssetPtr AssetDatabase::tryToLoadModel(const std::string& modelName, __out GfxModel& gfxModel, GfxEngine* gfxEngine)
{
	assert(gfxEngine);

	auto model = AssetTool::AssetDatabase().instance().tryToGetAsset(modelName);
	assert(model->type() == AssetTool::Asset::eModel);

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

	gfxModel.m_Meshes.resize(scene->mNumMeshes);
	for (uint32_t i = 0u; i < scene->mNumMeshes; ++i)
	{
		auto mesh = scene->mMeshes[i];
		assert(mesh);

		auto &curMesh = gfxModel.m_Meshes[i];

		std::vector<GfxModel::GfxVertex> vertices(mesh->mNumVertices);
		bool8_t hasNormal = mesh->HasNormals();
		bool8_t hasTangent = mesh->HasTangentsAndBitangents();
		bool8_t hasTexcoord = mesh->HasTextureCoords(0u);
		bool8_t hasVertexColor = mesh->HasVertexColors(0u);
		for (uint32_t j = 0u; j < mesh->mNumVertices; ++j) /// Async?
		{
			vertices[i].Position = Vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
			if (hasNormal)
			{
				vertices[i].Normal = Vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
			}
			if (hasTangent)
			{
				vertices[i].Tangent = Vec3(mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z);
				vertices[i].BiTangent = Vec3(mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z);
			}
			if (hasTexcoord)
			{
				vertices[i].UV = Vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
			}
			if (hasVertexColor)
			{
				vertices[i].VertexColor = Vec4(mesh->mColors[0][j].r, mesh->mColors[0][j].g, mesh->mColors[0][j].b, mesh->mColors[0][j].a);
			}
		}
		curMesh.VertexBuffer = gfxEngine->createVertexBuffer(eGpuReadWrite, sizeof(GfxModel::GfxVertex) * vertices.size(), vertices.data());

		std::vector<uint32_t> indices;
		for (uint32_t j = 0u; j < mesh->mNumFaces; ++j)
		{
			auto& face = mesh->mFaces[j];
			for (uint32_t k = 0u; k < face.mNumIndices; ++k)
			{
				indices.emplace_back(face.mIndices[k]);
			}
		}
		curMesh.IndexBuffer = gfxEngine->createIndexBuffer(eGpuReadWrite, sizeof(uint32_t) * indices.size(), indices.data());
		curMesh.IndexCount = (uint32_t)indices.size();

		if (scene->HasMaterials() && mesh->mMaterialIndex >= 0)
		{
			auto material = scene->mMaterials[mesh->mMaterialIndex];
			for (uint32_t j = aiTextureType_DIFFUSE; j < aiTextureType_UNKNOWN; ++j)
			{
				for (uint32_t k = 0u; k < material->GetTextureCount((aiTextureType)j); ++k)
				{
					aiString texPath;
					material->GetTexture((aiTextureType)j, k, &texPath);

					std::string texName(texPath.C_Str());
					texName = File::name(texName);
					auto texture = AssetTool::AssetDatabase().instance().tryToGetAsset(texName);
				}
			}
		}
	}

	return model;
}

namespaceEnd(AssetTool)