#include "Colorful/D3D/DXGI_Interface.h"
#include <ThirdParty/assimp/include/assimp/Importer.hpp>
#include <ThirdParty/assimp/include/assimp/scene.h>
#include <ThirdParty/assimp/include/assimp/postprocess.h>

NAMESPACE_START(Gfx)

class ModelLoader
{
public:
	static ModelDesc loadModel(const byte8_t* data, size_t size, const ModelOptions& options)
	{
		ModelDesc desc{};

		uint32_t flags = options.GenSmoothNormals ? aiProcess_GenSmoothNormals : (options.GenNormals ? aiProcess_GenNormals : 0u);
		flags |= 
			options.GenTangents ? aiProcess_CalcTangentSpace : 0u |
			options.GenUVs ? aiProcess_GenUVCoords : 0u |
			options.FlipWindingOrder ? aiProcess_FlipWindingOrder : 0u |
			options.Triangulate ? (aiProcess_Triangulate | aiProcess_SortByPType) : 0u |
			options.PreTransformVertices ? aiProcess_PreTransformVertices : 0u |
			options.MakeLeftHanded ? aiProcess_MakeLeftHanded : 0u |
			options.Optimize ? aiProcess_OptimizeMeshes : 0u |
			options.FlipUVs ? aiProcess_FlipUVs : 0u |
			options.GenBoundingBoxes ? aiProcess_GenBoundingBoxes : 0u;

		Assimp::Importer importer;
		auto scene = importer.ReadFileFromMemory(data, size, flags);
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
		desc.SubMeshes.resize(scene->mNumMeshes);
		desc.IndexFormat = EIndexFormat::UInt32;
		uint32_t primitiveTypes = 0u;

		auto getTexture = [&Textures = desc.Textures](const char8_t* path)->uint32_t {
			for (uint32_t i = 0u; i < Textures.size(); ++i)
			{
				if (_stricmp(path, Textures[i].c_str()) == 0)
				{
					return i;
				}
			}

			uint32_t index = static_cast<uint32_t>(Textures.size());
			Textures.emplace_back(std::move(std::string(path)));
			return index;
		};

		auto getTextureType = [](uint32_t type)->ModelDesc::ETextureType {
			switch (type)
			{
			default:
			case aiTextureType_NONE: 
			case aiTextureType_UNKNOWN:           return ModelDesc::ETextureType::Unknown;
			case aiTextureType_DIFFUSE:           return ModelDesc::ETextureType::Diffuse;
			case aiTextureType_SPECULAR:          return ModelDesc::ETextureType::Specular;
			case aiTextureType_AMBIENT:           return ModelDesc::ETextureType::Ambient;
			case aiTextureType_EMISSIVE:          return ModelDesc::ETextureType::Emissive;
			case aiTextureType_HEIGHT:            return ModelDesc::ETextureType::Height;
			case aiTextureType_NORMALS:           return ModelDesc::ETextureType::Normal;
			case aiTextureType_SHININESS:         return ModelDesc::ETextureType::Shininess;
			case aiTextureType_OPACITY:           return ModelDesc::ETextureType::Opacity;
			case aiTextureType_DISPLACEMENT:      return ModelDesc::ETextureType::Displacement;
			case aiTextureType_LIGHTMAP:          return ModelDesc::ETextureType::Lightmap;
			case aiTextureType_REFLECTION:        return ModelDesc::ETextureType::Reflection;
			case aiTextureType_BASE_COLOR:        return ModelDesc::ETextureType::Albedo;
			case aiTextureType_NORMAL_CAMERA:     return ModelDesc::ETextureType::Normal_Camera;
			case aiTextureType_EMISSION_COLOR:    return ModelDesc::ETextureType::Emission_Color;
			case aiTextureType_METALNESS:         return ModelDesc::ETextureType::Metalness;
			case aiTextureType_DIFFUSE_ROUGHNESS: return ModelDesc::ETextureType::Diffuse_Roughness;
			case aiTextureType_AMBIENT_OCCLUSION: return ModelDesc::ETextureType::Ambient_Occlusion;
			}
		};

		for (uint32_t i = 0u; i < scene->mNumMeshes; ++i)
		{
			auto mesh = scene->mMeshes[i];
			assert(mesh && mesh->HasPositions());

			uint32_t vertexUsageFlags = EVertexUsage::Position |
				mesh->HasNormals() ? EVertexUsage::Normal : 0u |
				mesh->HasTangentsAndBitangents() ? (EVertexUsage::Tangent | EVertexUsage::BiTangent) : 0u |
				mesh->HasTextureCoords(0u) ? EVertexUsage::Texcoord0 : 0u |
				mesh->HasTextureCoords(1u) ? EVertexUsage::Texcoord1 : 0u |
				mesh->HasTextureCoords(2u) ? EVertexUsage::Texcoord2 : 0u |
				mesh->HasTextureCoords(3u) ? EVertexUsage::Texcoord3 : 0u |
				mesh->HasVertexColors(0u) ? EVertexUsage::Color : 0u;
			if (!desc.VertexUsageFlags)
			{
				desc.setVertexUsageFlags(vertexUsageFlags);
			}
			assert(vertexUsageFlags == desc.VertexUsageFlags);

			if (desc.PrimitiveTopology == EPrimitiveTopology::Unknown)
			{
				primitiveTypes = mesh->mPrimitiveTypes;
				switch (mesh->mPrimitiveTypes)
				{
				case aiPrimitiveType_POINT:
					desc.PrimitiveTopology = EPrimitiveTopology::PointList;
					break;
				case aiPrimitiveType_LINE:
					desc.PrimitiveTopology = EPrimitiveTopology::LineList;
					break;
				case aiPrimitiveType_TRIANGLE:
					desc.PrimitiveTopology = EPrimitiveTopology::TriangleList;
					break;
				default:
					assert(0);
					break;
				}
			}
			assert(primitiveTypes == mesh->mPrimitiveTypes);

			auto& subMesh = desc.SubMeshes[i];
			subMesh.VertexCount = mesh->mNumVertices;
			desc.VertexCount += mesh->mNumVertices;
			subMesh.VertexBuffer = std::shared_ptr<byte8_t>(new byte8_t[mesh->mNumVertices * desc.VertexStride]());
			auto vertex = subMesh.VertexBuffer.get();
			auto vertex_o = vertex;
			assert(vertex);

			for (uint32_t j = 0u; j < mesh->mNumVertices; ++j) /// Async?
			{
				auto vec3 = reinterpret_cast<Vec3*>(vertex);
				vec3->x = mesh->mVertices[j].x; vec3->y = mesh->mVertices[j].y; vec3->z = mesh->mVertices[j].z;
				vertex += sizeof(Vec3);

				if (mesh->HasNormals())
				{
					vec3 = reinterpret_cast<Vec3*>(vertex);
					vec3->x = mesh->mNormals[j].x; vec3->y = mesh->mNormals[j].y; vec3->z = mesh->mNormals[j].z;
					vertex += sizeof(Vec3);
				}
				if (mesh->HasTangentsAndBitangents())
				{
					vec3 = reinterpret_cast<Vec3*>(vertex);
					vec3->x = mesh->mTangents[j].x; vec3->y = mesh->mTangents[j].y; vec3->z = mesh->mTangents[j].z;
					vertex += sizeof(Vec3);

					vec3->x = mesh->mBitangents[j].x; vec3->y = mesh->mBitangents[j].y; vec3->z = mesh->mBitangents[j].z;
					vertex += sizeof(Vec3);
				}
				for (uint32_t n = 0u; n < 4u; ++n)
				{
					if (mesh->HasTextureCoords(n))
					{
						auto vec2 = reinterpret_cast<Vec2*>(vertex);
						vec2->x = mesh->mTextureCoords[n][j].x; vec2->y = mesh->mTextureCoords[n][j].y;
						vertex += sizeof(Vec2);
					}
				}
				if (mesh->HasVertexColors(0u))
				{
					auto vec4 = reinterpret_cast<Vec4*>(vertex);
					vec4->x = mesh->mColors[0][j].r; vec4->y = mesh->mColors[0][j].g;
					vec4->z = mesh->mColors[0][j].b; vec4->w = mesh->mColors[0][j].a;
					vertex += sizeof(Vec4);
				}
				else
				{
					if (scene->HasMaterials())
					{
						aiColor4D color;
						scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, color);
						auto vec4 = reinterpret_cast<Vec4*>(vertex);
						vec4->x = color.r; vec4->y = color.g;
						vec4->z = color.b; vec4->w = color.a;
						vertex += sizeof(Vec4);
					}
				}

				assert(std::distance(vertex, vertex_o) == j * desc.VertexStride);
			}

			subMesh.FaceCount = mesh->mNumFaces;
			subMesh.BoneCount = mesh->mNumBones;
			desc.FaceCount += mesh->mNumFaces;

			std::vector<uint32_t> indices;
			for (uint32_t j = 0u; j < mesh->mNumFaces; ++j)
			{
				indices.insert(indices.end(), mesh->mFaces[j].mIndices, mesh->mFaces[j].mIndices + mesh->mFaces[j].mNumIndices);
			}
			desc.setIndices(i, indices);

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
					for (uint32_t k = 0u; k < material->GetTextureCount(static_cast<aiTextureType>(j)); ++k)
					{
						aiString path;
						material->GetTexture(static_cast<aiTextureType>(j), k, &path);
						subMesh.Textures.emplace_back(std::move(std::make_pair(getTextureType(j), getTexture(path.C_Str()))));
					}
				}
			}
		}

		desc.BoundingBox = AABB((vMax + vMin) * 0.5f, (vMax - vMin) * 0.5f);
		for (uint32_t i = 0u; i < desc.Textures.size(); ++i)
		{
			desc.Textures[i] = File::name(desc.Textures[i]);
		}

		return desc;
	}
protected:
private:
};

NAMESPACE_END(Gfx)