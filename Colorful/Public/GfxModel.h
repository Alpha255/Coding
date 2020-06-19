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

	struct GfxTexture
	{
		uint32_t Type = 0u;
		uint32_t Index = 0u;
	};

	struct GfxMaterial
	{
		std::vector<GfxTexture> Textures;
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

	void load(const std::string& modelName);
	void createAsQuad();

	void draw(const DXUTCamera& camera, const GfxViewport& viewport);
	void draw(const GfxPipelineState* state);
protected:
	friend class AssetTool::AssetDatabase;

	void initPipelineState();
private:
	bool8_t m_Valid = false;
	AABB m_BoundingBox;
	std::vector<GfxMesh> m_Meshes;
	std::array<std::vector<GfxTexturePtr>, 18u> m_Textures; /// aiTextureType_UNKNOWN
	GfxGpuBufferPtr m_UniformBuffer;
	static GfxPipelineState s_PipelineState;
	static GfxSamplerPtr s_LinearSampler;
};
