#include "AssetDatabase.h"
#include "Colorful/Public/GfxEngine.h"
#include <ThirdParty/assimp/include/assimp/Importer.hpp>
#include <ThirdParty/assimp/include/assimp/scene.h>
#include <ThirdParty/assimp/include/assimp/postprocess.h>

namespaceStart(AssetTool)

void AssetDatabase::tryToLoadModel(const std::string& modelName, __out GfxModel& gfxModel, GfxEngine* gfxEngine)
{
	assert(gfxEngine);

	Gear::CpuTimer timer;
	timer.start();

	auto model = AssetTool::AssetDatabase().instance().tryToGetAsset(modelName);
	assert(model->type() == AssetTool::Asset::eModel);

	///aiProcess_ConvertToLeftHanded;

	Assimp::Importer importer;
	auto scene = importer.ReadFile(model->fullPath(),
		(uint32_t)(
		aiPostProcessSteps::aiProcess_CalcTangentSpace |
		aiPostProcessSteps::aiProcess_Triangulate |
		aiPostProcessSteps::aiProcess_GenBoundingBoxes));
	///auto scene = importer.ReadFile(model->fullPath(), aiPostProcessSteps::aiProcess_Triangulate);
	if (!scene)
	{
		Logger::instance().log(Logger::eError, "Assimp: %s", importer.GetErrorString());
		return;
	}

	if (!scene->HasMeshes())
	{
		Logger::instance().log(Logger::eError, "The model %s is empty", model->name().c_str());
		return;
	}

	timer.stop();
	Logger::instance().log(Logger::eInfo, format("Read %s takes %.2fs", model->name().c_str(), timer.elapsedTime()));
	
	timer.reset();
	timer.start();

	Vec3 vMin(std::numeric_limits<float32_t>::max(), std::numeric_limits<float32_t>::max(), std::numeric_limits<float32_t>::max());
	Vec3 vMax(std::numeric_limits<float32_t>::min(), std::numeric_limits<float32_t>::min(), std::numeric_limits<float32_t>::min());
	std::set<std::string> loadedTextures;

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
			vertices[j].Position = Vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
			if (hasNormal)
			{
				vertices[j].Normal = Vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
			}
			if (hasTangent)
			{
				vertices[j].Tangent = Vec3(mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z);
				vertices[j].BiTangent = Vec3(mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z);
			}
			if (hasTexcoord)
			{
				vertices[j].UV = Vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
			}
			if (hasVertexColor)
			{
				vertices[j].VertexColor = Vec4(mesh->mColors[0][j].r, mesh->mColors[0][j].g, mesh->mColors[0][j].b, mesh->mColors[0][j].a);
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
		auto extent = (mesh->mAABB.mMax + mesh->mAABB.mMin) * 0.5f;
		auto center = (mesh->mAABB.mMax - mesh->mAABB.mMin) * 0.5f;
		curMesh.BoundingBox = AABB(Vec3(center.x, center.y, center.z), Vec3(extent.x, extent.y, extent.z));
		vMin = Math::getMin(vMin, Vec3(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z));
		vMax = Math::getMax(vMax, Vec3(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z));

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
					if (loadedTextures.find(texName) == loadedTextures.end())
					{
						texName = File::name(texName);
						auto gfxTexture = gfxEngine->createTexture(texName);
						gfxModel.m_Textures[j].emplace_back(std::move(gfxTexture));

						GfxModel::GfxTexture texture
						{
							(GfxModel::GfxTexture::eTextureType)j,
							(uint32_t)(gfxModel.m_Textures[j].size() - 1u),
							texName
						};
						curMesh.Material.Textures.emplace_back(std::move(texture));

						loadedTextures.emplace(std::move(texName));
					}
				}
			}
		}
	}

	gfxModel.m_BoundingBox = AABB((vMax + vMin) * 0.5f, (vMax - vMin) * 0.5f);

	timer.stop();
	Logger::instance().log(Logger::eInfo, format("Build model %s takes %.2fs\n", model->name().c_str(), timer.elapsedTime()));
	gfxModel.m_Valid = true;
}

namespaceEnd(AssetTool)