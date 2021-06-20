#pragma once

#include "Colorful/Gfx/AssetTool/Material.h"
#include "Colorful/Gfx/GfxRenderState.h"

NAMESPACE_START(Gfx)

struct MeshDesc
{
	enum class ETextureType
	{
		Diffuse,
		Specular,
		Ambient,
		Emissive,
		Height,
		Normal,
		Shininess,
		Opacity,
		Displacement,
		Lightmap,
		Reflection,
		Albedo,
		Normal_Camera,
		Emission_Color,
		Metalness,
		Diffuse_Roughness,
		Ambient_Occlusion,
		Unknown
	};

	uint32_t VertexCount = 0u;
	uint32_t IndexCount = 0u;
	uint32_t FaceCount = 0u;
	uint32_t BoneCount = 0u;
	AABB BoundingBox;

	std::shared_ptr<byte8_t> VertexBuffer;
	size_t VertexBufferSize = 0u;

	std::vector<uint32_t> IndexBufferUInt32;
	std::vector<uint16_t> IndexBufferUInt16;
	std::array<uint32_t, static_cast<size_t>(ETextureType::Unknown)> Textures;
};

struct ModelDesc : public MeshDesc
{
	EIndexFormat IndexFormat = EIndexFormat::UInt32;
	EPrimitiveTopology PrimitiveTopology = EPrimitiveTopology::Unknown;
	uint32_t VertexUsageFlags = 0u;
	uint32_t VertexStride = 0u;

	std::vector<MeshDesc> MeshDescs;
	std::vector<std::string> Textures;
	VertexInputLayoutDesc InputLayout;
	std::string Name;

	void buildInputLayout();
};

class MeshAttributes
{
public:
	MeshAttributes() = default;
	MeshAttributes(uint32_t vertexCount, uint32_t indexCount, uint32_t faceCount, uint32_t boneCount, const AABB& boundingBox)
		: m_VertexCount(vertexCount)
		, m_IndexCount(indexCount)
		, m_FaceCount(faceCount)
		, m_BoneCount(boneCount)
		, m_BoundingBox(boundingBox)
	{
	}
	virtual ~MeshAttributes() = default;

	uint32_t vertexCount() const
	{
		return m_VertexCount;
	}

	uint32_t indexCount() const
	{
		return m_IndexCount;
	}

	uint32_t boneCount() const
	{
		return m_BoneCount;
	}

	uint32_t faceCount() const
	{
		return m_FaceCount;
	}

	const AABB& boundingBox() const
	{
		return m_BoundingBox;
	}
protected:
	uint32_t m_VertexCount = 0u;
	uint32_t m_IndexCount = 0u;
	uint32_t m_FaceCount = 0u;
	uint32_t m_BoneCount = 0u;
	AABB m_BoundingBox;
};

class Mesh : public MeshAttributes
{
public:
	using MeshAttributes::MeshAttributes;
protected:
	friend class Model;
	GpuBufferPtr m_VertexBuffer;
	GpuBufferPtr m_IndexBuffer;
	std::array<uint32_t, static_cast<size_t>(MeshDesc::ETextureType::Unknown)> m_Textures;
};

class Model : public MeshAttributes, public GfxResource
{
public:
	struct LoadingSettings
	{
		bool8_t GenNormals = false;
		bool8_t GenSmoothNormals = false;
		bool8_t GenTangents = false;
		bool8_t GenUVs = false;
		bool8_t FlipWindingOrder = false; /// default is CCW
		bool8_t Triangulate = false;
		bool8_t PreTransformVertices = false;
		bool8_t MakeLeftHanded = false;
		bool8_t Optimize = false;
		bool8_t FlipUVs = false;
		bool8_t GenBoundingBoxes = true;
	};

	Model(ModelDesc& desc);

	static std::shared_ptr<Model> EXPORT_API cube(float32_t length);

	GraphicsPipelineState& graphicsPipelineState(bool8_t usePBR = false)
	{
		return usePBR ? *m_PBRPipelineState : *m_DefaultPipelineState;
	}

	void setMaterial(MaterialPtr material)
	{
		m_DefaultPipelineState->setMaterial(material);
		m_PBRPipelineState->setMaterial(material);
	}
protected:
	friend class ModelLoader;
private:
	GraphicsPipelineStatePtr m_DefaultPipelineState;
	GraphicsPipelineStatePtr m_PBRPipelineState;

	std::vector<Mesh> m_Meshes;
	std::vector<TexturePtr> m_Textures;
};

NAMESPACE_END(Gfx)