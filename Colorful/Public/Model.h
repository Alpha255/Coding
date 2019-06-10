#pragma once

#include "Base/DXUTCamera.h"
#include "Definitions.h"
#include "Geometry.h"
#include "Lighting.h"

NamespaceBegin(Geometry)

class Model
{
public:
	struct SubModel
	{
		uint32_t VertexBuffer = 0U;
		uint32_t IndexBuffer = 0U;

		uint32_t IndexCount = 0U;
		uint32_t StartIndex = 0U;
		int32_t VertexOffset = 0;
	
		eRPrimitiveTopology PrimitiveType = eTriangleList;

		Material Mat;
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

	inline bool IsValid() const
	{
		return m_Valid;
	}

	void CreateAsCube(float width, float height, float depth);
	void CreateAsSphere(float radius, uint32_t slice, uint32_t stack);
	void CreateAsGrid(float width, float depth, uint32_t m, uint32_t n);
	void CreateAsQuad(float left, float top, float width, float height);
	void CreateFromFile(const std::string &fileName);

	void AddBuffer(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

	inline void AddBuffer(const RBuffer &vertexBuffer, const RBuffer &indexBuffer)
	{
		assert(vertexBuffer.IsValid() && indexBuffer.IsValid());
		m_VertexBuffers.emplace_back(vertexBuffer);
		m_IndexBuffers.emplace_back(indexBuffer);
	}

	inline void AppendSubModel(const SubModel &subModel)
	{
		m_SubModels.emplace_back(subModel);
	}

	void Draw(const DXUTCamera &camera, bool bDrawBoundingBox = false);
protected:
	void DrawBoundingBox(const DXUTCamera &camera);

private:
	std::vector<SubModel> m_SubModels;
	std::vector<RBuffer> m_VertexBuffers;
	std::vector<RBuffer> m_IndexBuffers;
	RInputLayout m_InputLayout;

	Box m_BoundingBox;

	bool m_Valid = false;
	bool m_HasBoundingBox = false;
};

NamespaceEnd(Geometry)
