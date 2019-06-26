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

	void Draw(const DXUTCamera &camera, bool bDrawBoundingBox = false);
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
