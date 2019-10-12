#pragma once

#include "Base/DXUTCamera.h"
#include "Definitions.h"
#include "Geometry.h"
#include "Lighting.h"
#include <DirectXTK/Inc/Model.h>

NamespaceBegin(Geometry)

class Model
{
public:
#if 0
	std::unique_ptr<DirectX::Model> pModel;
#endif
	enum eBufferType
	{
		eVertexBuffer,
		eIndexBuffer,
		eTypeCount
	};

	struct SubModel
	{
		uint32_t VertexBuffer = UINT32_MAX;
		uint32_t IndexBuffer = UINT32_MAX;
		uint32_t InputLayout = UINT32_MAX;

		uint32_t IndexCount = UINT32_MAX;
		uint32_t StartIndex = UINT32_MAX;
		int32_t VertexOffset = INT32_MAX;
	
		uint32_t MaterialIndex = UINT32_MAX;
		eRPrimitiveTopology PrimitiveType = eTriangleList;
		eRFormat IndexFormat = eR32_UInt;
	};

	Model() = default;
	~Model() = default;

	inline bool HasBoundingBox() const
	{
		return m_HasBoundingBox;
	}

	inline Box GetBoundingBox() const
	{
		return m_BoundingBox;
	}

	inline void UpdateBoundingBox(const Vec3 &min, const Vec3 &max)
	{
		m_BoundingBox = Box(Vec3::Min(min, m_BoundingBox.GetMin()), Vec3::Max(max, m_BoundingBox.GetMax()));
		m_HasBoundingBox = true;
	}

	inline bool IsValid() const
	{
		return m_Valid;
	}

	void CreateAsCube(float width, float height, float depth);
	void CreateAsSphere(float radius, uint32_t slice, uint32_t stack);
	void CreateAsGrid(float width, float depth, uint32_t m, uint32_t n);
	void CreateAsQuad(float left, float top, float width, float height);
	void CreateFromFile(const std::string &fileName);

	void AddBuffer(const std::vector<Vertex> &vertices);
	void AddBuffer(const std::vector<uint32_t> &indices);

	inline void AddBuffer(const RBuffer &buffer, eBufferType type)
	{
		assert(buffer.IsValid() && type < eTypeCount);
		if (type == eVertexBuffer)
		{
			m_VertexBuffers.emplace_back(buffer);
		}
		else if (type == eIndexBuffer)
		{
			m_IndexBuffers.emplace_back(buffer);
		}
	}

	inline void AddMaterial(const Material &mat)
	{
		m_Materials.emplace_back(mat);
	}

	inline void AddMaterials(const std::vector<Material> &mats)
	{
		m_Materials.insert(m_Materials.end(), mats.cbegin(), mats.cend());
	}

	inline void AddInputLayout(const RInputLayout &layout)
	{
		m_InputLayouts.emplace_back(layout);
	}

	void AppendSubModel(const SubModel &subModel, bool bUseDefaultLayout = true);

	void Draw(const DXUTCamera &camera, bool bDrawBoundingBox = false, RContext *pContext = nullptr);
protected:
	void DrawBoundingBox(const DXUTCamera &camera);

private:
	std::vector<SubModel> m_SubModels;
	std::vector<RBuffer> m_VertexBuffers;
	std::vector<RBuffer> m_IndexBuffers;
	std::vector<RInputLayout> m_InputLayouts;
	std::vector<Material> m_Materials;

	RInputLayout m_DefaultLayout;

	Box m_BoundingBox;

	bool m_Valid = false;
	bool m_HasBoundingBox = false;
};

NamespaceEnd(Geometry)

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

		m_AABBBox = gear::math::aabb();
		m_OBBBox = gear::math::obb();
	}

	uint32_t getVertexStride() const
	{
		uint32_t stride = 0u;
		for each (auto layout in m_VertexLayouts)
		{
			stride += layout.Stride;
		}

		assert(stride % sizeof(float32_t) == 0u);
		return stride;
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

	gear::math::aabb m_AABBBox;
	gear::math::obb m_OBBBox;
};
