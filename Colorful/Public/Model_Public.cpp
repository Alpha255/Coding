#include "Model_Public.h"
#include "Definitions.h"

std::vector<vertexLayout> vertexLayout::make(const std::vector<std::pair<eVertexUsage, uint32_t>> &usageFormatPairs)
{
	std::vector<vertexLayout> result(usageFormatPairs.size());
	uint32_t offset = 0u;
	for (uint32_t i = 0u; i < usageFormatPairs.size(); ++i)
	{
		result[i].Usage = usageFormatPairs[i].first;
		result[i].Format = usageFormatPairs[i].second;
		result[i].Offset += offset;

		switch (result[i].Usage)
		{
		case eNone:
		default:
			assert(0);
			break;
		case ePosition:
		case eNormal:
		case eTangent:
			result[i].Stride = sizeof(float32_t) * 3u;
			break;
		case eTexcoord:
			result[i].Stride = sizeof(float32_t) * 2u;
			break;
		case eColor:
			result[i].Stride = sizeof(float32_t) * 3u;
			break;
		}
		offset += result[i].Stride;
	}

	return result;
}

void model::createFromFile(const std::string &modelName)
{
	init();
	std::string ext = file::getFileExtension(modelName, true);
	if (ext == ".sdkmesh")
	{

	}
	else if (ext == ".obj" || ext == ".dae")
	{
		assetTool::loadModelFromFile(modelName, *this);
	}
	else
	{
		assert(0);
	}
}

void model::createAsBox(float32_t width, float32_t height, float32_t depth)
{
	init();
	m_VertexLayouts = vertexLayout::make({
		{ vertexLayout::ePosition, eRGB32_Float },
		{ vertexLayout::eNormal, eRGB32_Float },
		{ vertexLayout::eTexcoord, eRG32_Float },
		});

	float32_t w = width * 0.5f;
	float32_t h = height * 0.5f;
	float32_t d = depth * 0.5f;
	m_Vertices = std::vector<float32_t>{
		-w, -h, -d, +0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		-w, +h, -d, +0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		+w, +h, -d, +0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		+w, -h, -d, +0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		-w, -h, +d, +0.0f, 0.0f, +1.0f, 1.0f, 1.0f,
		+w, -h, +d, +0.0f, 0.0f, +1.0f, 0.0f, 1.0f,
		+w, +h, +d, +0.0f, 0.0f, +1.0f, 0.0f, 0.0f,
		-w, +h, +d, +0.0f, 0.0f, +1.0f, 1.0f, 0.0f,
		-w, +h, -d, +0.0f, 1.0f, +0.0f, 0.0f, 1.0f,
		-w, +h, +d, +0.0f, 1.0f, +0.0f, 0.0f, 0.0f,
		+w, +h, +d, +0.0f, 1.0f, +0.0f, 1.0f, 0.0f,
		+w, +h, -d, +0.0f, 1.0f, +0.0f, 1.0f, 1.0f,
		-w, -h, -d, -1.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		+w, -h, -d, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		+w, -h, +d, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-w, -h, +d, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		-w, -h, +d, -1.0f, 0.0f, +0.0f, 0.0f, 1.0f,
		-w, +h, +d, -1.0f, 0.0f, +0.0f, 0.0f, 0.0f,
		-w, +h, -d, -1.0f, 0.0f, +0.0f, 1.0f, 0.0f,
		-w, -h, -d, -1.0f, 0.0f, +0.0f, 1.0f, 1.0f,
		+w, -h, -d, +1.0f, 0.0f, +0.0f, 0.0f, 1.0f,
		+w, +h, -d, +1.0f, 0.0f, +0.0f, 0.0f, 0.0f,
		+w, +h, +d, +1.0f, 0.0f, +0.0f, 1.0f, 0.0f,
		+w, -h, +d, +1.0f, 0.0f, +0.0f, 1.0f, 1.0f,
	};

	m_Indices = std::vector<uint32_t>
	{
		/// front
		0, 1, 2, 0, 2, 3,
		/// back
		4, 5, 6, 4, 6, 7,
		/// top
		8, 9, 10, 8, 10, 11,
		/// bottom
		12, 13, 14, 12, 14, 15,
		/// left
		16, 17, 18, 16, 18, 19,
		/// right
		20, 21, 22, 20, 22, 23
	};

	m_SubModels.emplace_back(subModel{
		0u,
		(uint32_t)m_Indices.size(),
		0u,
		UINT32_MAX
		});
}

void model::computeBoundingBox()
{
}