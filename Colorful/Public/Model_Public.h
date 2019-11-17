#pragma once

#include "gear/gear.h"
#include "Colorful/AssetTool/AssetTool.h"

struct vertexLayout
{
	enum eVertexUsage : uint32_t
	{
		eNone,
		ePosition,
		eNormal,
		eTangent,
		eTexcoord,
		eColor
	};

	eVertexUsage Usage = eNone;
	uint32_t Stride = 0u;
	uint32_t Offset = 0u;
	uint32_t Format = 0u;

	static std::vector<vertexLayout> make(const std::vector<std::pair<eVertexUsage, uint32_t>> &usageFormatPairs);
};
typedef std::vector<vertexLayout> vertexLayouts;

class material
{

};

class model
{
public:
	struct fatVertex
	{
		vec3 Pos;
		vec3 Normal;
		vec3 Tangent;
		vec2 UV;
		vec3 Color;
	};

	struct subModel
	{
		uint32_t VertexOffset = UINT32_MAX;
		uint32_t IndexCount = UINT32_MAX;
		uint32_t IndexOffset = UINT32_MAX;
		uint32_t MaterialIndex = UINT32_MAX;
	};

	void createFromFile(const std::string &modelName);

	inline void init()
	{
		m_Vertices.clear();
		m_Indices.clear();
		m_VertexLayouts.clear();
		m_SubModels.clear();

		m_AABB = math::aabb();
		m_OBB = math::obb();
	}

	uint32_t getVertexStride()
	{
		if (UINT32_MAX == m_VertexStride)
		{
			m_VertexStride = 0u;
			for each (auto layout in m_VertexLayouts)
			{
				m_VertexStride += layout.Stride;
			}

			assert(m_VertexStride % sizeof(float32_t) == 0u);
		}
		return m_VertexStride;
	}

	const std::vector<float32_t> &getVertices() const
	{
		return m_Vertices;
	}

	const std::vector<uint32_t> &getIndices() const
	{
		return m_Indices;
	}

	const std::vector<subModel> &getSubModels() const
	{
		return m_SubModels;
	}

	const vertexLayouts &getVertexLayouts() const
	{
		return m_VertexLayouts;
	}

	math::aabb getAABB() const
	{
		return m_AABB;
	}

	math::obb getOBB() const
	{
		return m_OBB;
	}

	void computeBoundingBox();
	void createAsBox(float32_t width, float32_t height, float32_t depth);
protected:
	friend void assetTool::loadModelFromFile(const std::string &, model &);
private:
	std::vector<float32_t> m_Vertices;
	std::vector<uint32_t> m_Indices;

	vertexLayouts m_VertexLayouts;
	std::vector<subModel> m_SubModels;
	std::vector<material> m_Materials;

	math::aabb m_AABB;
	math::obb m_OBB;

	uint32_t m_VertexStride = UINT32_MAX;
};
