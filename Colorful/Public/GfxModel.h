#pragma once

#include "Colorful/Public/GfxRenderState.h"

NAMESPACE_START(Gfx)

struct ModelDesc
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

	static constexpr size_t indexStride(EIndexFormat format)
	{
		return format == EIndexFormat::UInt16 ? sizeof(uint16_t) : sizeof(uint32_t);
	}

	struct SubMeshDesc
	{
		uint32_t VertexCount = 0;
		uint32_t IndexCount = 0u;
		uint32_t FaceCount = 0u;
		uint32_t BoneCount = 0u;
		AABB BoundingBox;

		std::shared_ptr<byte8_t> VertexBuffer = nullptr;
		std::shared_ptr<byte8_t> IndexBuffer = nullptr;
		std::vector<std::pair<ModelDesc::ETextureType, uint32_t>> Textures;
	};

	void setVertexUsageFlags(uint32_t flags)
	{
		assert(flags && !VertexUsageFlags);

		VertexUsageFlags = flags;
		if (VertexUsageFlags & EVertexUsage::Position)
		{
			VertexStride += sizeof(Vec3);
		}
		if (VertexUsageFlags & EVertexUsage::Normal)
		{
			VertexStride += sizeof(Vec3);
		}
		if (VertexUsageFlags & EVertexUsage::Tangent)
		{
			VertexStride += sizeof(Vec3);
		}
		if (VertexUsageFlags & EVertexUsage::BiNormal)
		{
			VertexStride += sizeof(Vec3);
		}
		if (VertexUsageFlags & EVertexUsage::BiTangent)
		{
			VertexStride += sizeof(Vec3);
		}
		if (VertexUsageFlags & EVertexUsage::Texcoord0)
		{
			VertexStride += sizeof(Vec2);
		}
		if (VertexUsageFlags & EVertexUsage::Texcoord1)
		{
			VertexStride += sizeof(Vec2);
		}
		if (VertexUsageFlags & EVertexUsage::Texcoord2)
		{
			VertexStride += sizeof(Vec2);
		}
		if (VertexUsageFlags & EVertexUsage::Texcoord3)
		{
			VertexStride += sizeof(Vec2);
		}
		if (VertexUsageFlags & EVertexUsage::Color)
		{
			VertexStride += sizeof(Vec4);
		}
		if (VertexUsageFlags & EVertexUsage::Weight)
		{
			VertexStride += sizeof(Vec3);
		}
		assert(VertexStride);
	}

	template<class Vertex>
	void setVertices(uint32_t subMesh, const std::vector<Vertex>& vertices)
	{
		assert(subMesh < MeshCount);
	}

	template<class IndexFormat>
	void setIndices(uint32_t subMesh, const std::vector<IndexFormat>& indices)
	{
		assert(subMesh < MeshCount);
	}

	uint32_t VertexUsageFlags = 0u;
	uint32_t VertexStride = 0u;
	uint32_t MeshCount = 0u;
	uint32_t VertexCount = 0u;
	uint32_t IndexCount = 0u;
	uint32_t FaceCount = 0u;
	uint32_t AnimationCount = 0u;
	EIndexFormat IndexFormat = EIndexFormat::UInt32;
	EPrimitiveTopology PrimitiveTopology = EPrimitiveTopology::TriangleList;
	AABB BoundingBox;

	std::vector<SubMeshDesc> SubMeshes;
	std::vector<std::string> Textures;
};

DECLARE_SHARED_PTR(Model)
class EXPORT_API Model
{
public:
	Model(const ModelDesc& desc)
	{
	}

	static Model createCube(float32_t length);
protected:
private:
};

NAMESPACE_END(Gfx)
