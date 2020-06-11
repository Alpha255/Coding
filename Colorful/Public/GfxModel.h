#pragma once

#include "Colorful/Public/GfxRenderState.h"

namespace AssetTool
{
	class AssetDatabase;
}

class GfxModel
{
public:
	struct GfxVertex
	{
		Vec3 Position;
		Vec3 Normal;
		Vec3 Tangent;
		Vec3 BiTangent;
		Vec2 UV;
		Vec4 VertexColor;
	};

	struct GfxMaterial
	{
	};

	struct GfxMesh
	{
		GfxGpuBufferPtr VertexBuffer = nullptr;
		GfxGpuBufferPtr IndexBuffer = nullptr;
		uint32_t IndexCount = 0u;

		GfxMaterial Material;

		AABB BoundingBox;
	};

	struct GfxMeshPart
	{
		uint32_t VertexOffset = 0u;
		uint32_t IndexOffset = 0u;
		uint32_t NumIndices = 0u;
		uint32_t MaterialIndex = 0u;
	};

	void load(const std::string& modelName, class GfxEngine* gfxEngine);
protected:
	friend class AssetTool::AssetDatabase;
private:
	AABB m_BoundingBox;
	std::vector<GfxMesh> m_Meshes;
};
