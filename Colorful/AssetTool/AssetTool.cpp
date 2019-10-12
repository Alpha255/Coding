#include "AssetTool.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "Colorful/Public/Model.h"

namespaceStart(assetTool)

void loadModelFromFile(const std::string &modelName, model &outModel)
{
	auto assetFile = gear::assetBucket::instance().getAsset(modelName);
	assert(assetFile && assetFile->getType() == gear::assetFile::eStaticMesh);

	const int32_t readFlags =
		aiProcess_FlipWindingOrder |
		aiProcess_Triangulate |
		aiProcess_PreTransformVertices |
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals;

	Assimp::Importer importer;
	auto const scene = importer.ReadFile(assetFile->getFullPath(), readFlags);
	assert(scene);

	outModel.m_VertexLayouts = vertexLayout::make({
		{ vertexLayout::ePosition, eRGB32_Float },
		{ vertexLayout::eNormal, eRGB32_Float },
		{ vertexLayout::eTangent, eRGB32_Float },
		{ vertexLayout::eTexcoord, eRG32_Float },
		{ vertexLayout::eColor, eRGB32_Float },
		});

	uint32_t vertexCount = 0u, indexCount = 0u;
	outModel.m_SubModels.resize(scene->mNumMeshes);
	const aiVector3D zero(0.0f, 0.0f, 0.0f);
	std::vector<vec3> vertices;

	for (uint32_t i = 0u; i < scene->mNumMeshes; ++i)
	{
		auto const mesh = scene->mMeshes[i];
		outModel.m_SubModels[i].VertexOffset = vertexCount;
		outModel.m_SubModels[i].IndexOffset = indexCount;

		vertexCount += mesh->mNumVertices;

		aiColor3D color(0.f, 0.f, 0.f);
		scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);

		for (uint32_t j = 0u; j < mesh->mNumVertices; ++j)
		{
			const aiVector3D* pPos = &(mesh->mVertices[j]);
			const aiVector3D* pNormal = &(mesh->mNormals[j]);
			const aiVector3D* pTangent = (mesh->HasTangentsAndBitangents()) ? &(mesh->mTangents[j]) : &zero;
			const aiVector3D* pTexCoord = (mesh->HasTextureCoords(0)) ? &(mesh->mTextureCoords[0][j]) : &zero;

			std::vector<float32_t> vertex
			{
				pPos->x, pPos->y, pPos->z,
				pNormal->x, pNormal->y, pNormal->z,
				pTangent->x, pTangent->y, pTangent->z,
				pTexCoord->x, pTexCoord->y,
				color.r, color.g, color.b
			};
			outModel.m_Vertices.insert(outModel.m_Vertices.cend(), vertex.cbegin(), vertex.cend());
			vertices.emplace_back(vec3(pPos->x, pPos->y, pPos->z));
		}

		uint32_t indexOffset = (uint32_t)outModel.m_Indices.size();
		for (uint32_t j = 0u; j < mesh->mNumFaces; ++j)
		{
			auto const &face = mesh->mFaces[j];
			if (face.mNumIndices != 3u)
			{
				continue;
			}

			outModel.m_Indices.emplace_back(indexOffset + face.mIndices[0u]);
			outModel.m_Indices.emplace_back(indexOffset + face.mIndices[1u]);
			outModel.m_Indices.emplace_back(indexOffset + face.mIndices[2u]);

			outModel.m_SubModels[i].IndexCount += 3u;
			indexCount += 3u;
		}
	}

	outModel.m_AABBBox = gear::math::aabb::createFromVertices(vertices);
}

/// Standard, Portable Intermediate Representation - V
std::vector<uint32_t> compileVkShader(const std::string &fileName, const std::string &entryPoint, uint32_t shaderStage)
{
	std::vector<uint32_t> spirv;
	auto shaderFile = assetBucket::instance().getAsset(fileName);
	assert(shaderFile);

	std::string inputFile(shaderFile->getFullPath());
	std::string outputFile = file::stripExtension(inputFile) + ".spirv";

	char8_t parserPath[MAX_PATH] = {};
	verifyWin(::GetEnvironmentVariableA("VULKAN_SDK", parserPath, MAX_PATH) != 0);
	std::string commandline(parserPath);
	commandline += "\\Bin\\glslangValidator ";

	switch (shaderStage)
	{
	case eVertexShader:
		commandline += "-S vert ";
		commandline += "-DVertexShader ";
		break;
	case ePixelShader:
		commandline += "-S frag ";
		commandline += "-DFragmentShader ";
		break;
	default:
		assert(0);
		break;
	}

	commandline += "-e ";
	commandline += entryPoint;
	commandline += " ";

#if defined(_DEBUG)
	commandline += "-g ";
#endif
	commandline += "-V ";

	commandline += "-o ";
	commandline += outputFile;
	commandline += " ";
	commandline += inputFile;

	assert(gear::executeProcess(commandline, true));

	gear::fileIO shaderBinary(outputFile);
	size_t shaderBinarySize = shaderBinary.getSize();
	assert((shaderBinarySize % sizeof(uint32_t)) == 0u);
	spirv.resize(shaderBinarySize / sizeof(uint32_t));
	verify(memcpy_s((void *)spirv.data(), shaderBinarySize, (void *)shaderBinary.getData(gear::fileIO::eBinary).get(), shaderBinarySize) == 0);

	return spirv;
}

namespaceEnd(assetTool)
