#include "Colorful/D3D/DXGI_Interface.h"
#include <ThirdParty/assimp/include/assimp/Importer.hpp>
#include <ThirdParty/assimp/include/assimp/scene.h>
#include <ThirdParty/assimp/include/assimp/postprocess.h>

NAMESPACE_START(Gfx)

class ModelLoader
{
public:
	static ModelDesc loadModel(const byte8_t* data, size_t size)
	{
		ModelDesc desc{};
		///aiProcess_ConvertToLeftHanded;

		Assimp::Importer importer;
		//auto scene = importer.ReadFile(model->fullPath(),
		//	(uint32_t)(
		//	aiPostProcessSteps::aiProcess_CalcTangentSpace |
		//	aiPostProcessSteps::aiProcess_Triangulate |
		//	aiPostProcessSteps::aiProcess_GenBoundingBoxes));
		auto scene = importer.ReadFileFromMemory(
			data,
			size,
			(uint32_t)(
				aiProcess_FlipWindingOrder |
				aiProcess_Triangulate |
				aiProcess_PreTransformVertices |
				aiProcess_CalcTangentSpace |
				aiProcess_GenSmoothNormals));
		if (!scene)
		{
			LOG_ERROR("ModelLoader:: %s", importer.GetErrorString());
			return desc;
		}

		if (!scene->HasMeshes())
		{
			LOG_ERROR("ModelLoader:: The model has no vertices");
			return desc;
		}

		Vec3 vMin(std::numeric_limits<float32_t>::max());
		Vec3 vMax(std::numeric_limits<float32_t>::min());
		std::vector<std::string> textures;

		desc.MeshCount = scene->mNumMeshes;
		desc.SubMeshes.resize(scene->mNumMeshes);
		for (uint32_t i = 0u; i < scene->mNumMeshes; ++i)
		{
			auto mesh = scene->mMeshes[i];
			assert(mesh);

			auto& subMesh = desc.SubMeshes[i];

			subMesh.HasNormals = mesh->HasNormals();
			subMesh.HasTangents = mesh->HasTangentsAndBitangents();
			subMesh.HasUVs = mesh->HasTextureCoords(0u);
			subMesh.HasVertexColors = mesh->HasVertexColors(0u);

			subMesh.Vertices.resize(mesh->mNumVertices);
			if (subMesh.HasNormals)
			{
				subMesh.Normals.resize(mesh->mNumVertices);
			}
			if (subMesh.HasTangents)
			{
				subMesh.Tangents.resize(mesh->mNumVertices);
			}
			if (subMesh.HasUVs)
			{
				subMesh.UVs.resize(mesh->mNumVertices);
			}
			if (subMesh.HasVertexColors)
			{
				subMesh.VertexColors.resize(mesh->mNumVertices);
			}
			for (uint32_t j = 0u; j < mesh->mNumVertices; ++j) /// Async?
			{
				subMesh.Vertices[j] = Vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
				if (subMesh.HasNormals)
				{
					subMesh.Normals[j] = Vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
				}
				if (subMesh.HasTangents)
				{
					subMesh.Tangents[j] = Vec3(mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z);
					subMesh.BiTangents[j] = Vec3(mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z);
				}
				if (subMesh.HasUVs)
				{
					subMesh.UVs[j] = Vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
				}
				if (subMesh.HasVertexColors)
				{
					subMesh.VertexColors[j] = Vec4(mesh->mColors[0][j].r, mesh->mColors[0][j].g, mesh->mColors[0][j].b, mesh->mColors[0][j].a);
				}
				else
				{
					if (scene->HasMaterials())
					{
						aiColor3D color;
						scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, color);
						subMesh.VertexColors[j] = Vec4(color.r, color.g, color.b, 1.0f);
					}
				}
			}

			subMesh.FaceCount = mesh->mNumFaces;
			subMesh.HasBones = mesh->HasBones();
			subMesh.BoneCount = mesh->mNumBones;

			for (uint32_t j = 0u; j < mesh->mNumFaces; ++j)
			{
				auto& face = mesh->mFaces[j];
				for (uint32_t k = 0u; k < face.mNumIndices; ++k)
				{
					subMesh.Indices.push_back(face.mIndices[k]);
				}
			}
			subMesh.IndexCount = static_cast<uint32_t>(subMesh.Indices.size());
			auto extent = (mesh->mAABB.mMax + mesh->mAABB.mMin) * 0.5f;
			auto center = (mesh->mAABB.mMax - mesh->mAABB.mMin) * 0.5f;
			subMesh.BoundingBox = AABB(Vec3(center.x, center.y, center.z), Vec3(extent.x, extent.y, extent.z));
			vMin = Math::min(vMin, Vec3(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z));
			vMax = Math::max(vMax, Vec3(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z));

			if (scene->HasMaterials() && mesh->mMaterialIndex >= 0)
			{
				auto material = scene->mMaterials[mesh->mMaterialIndex];
				for (uint32_t j = aiTextureType_DIFFUSE; j < aiTextureType_UNKNOWN; ++j)
				{
					for (uint32_t k = 0u; k < material->GetTextureCount((aiTextureType)j); ++k)
					{
						aiString texPath;
						material->GetTexture((aiTextureType)j, k, &texPath);
					}
				}
			}
		}

		desc.BoundingBox = AABB((vMax + vMin) * 0.5f, (vMax - vMin) * 0.5f);

		return desc;
	}
protected:
private:
};

NAMESPACE_END(Gfx)