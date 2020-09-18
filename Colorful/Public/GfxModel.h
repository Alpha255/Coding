#pragma once

#if 0

#include "Colorful/Public/GfxRenderState.h"

namespace AssetTool
{
	class AssetDatabase;
}

#if 0
template<size_t Dimension>
struct Vec
{
};

#define VecTypeName(Dimension)                \
template<>                                    \
struct Vec<Dimension> : public Vec##Dimension \
{                                             \
};                                            \
using Vector##Dimension = Vec<Dimension>;

VecTypeName(2)
VecTypeName(3)
VecTypeName(4)

struct GfxVertexLayout
{
	GfxVertexLayout(uint32_t layout, uint32_t vertexCount)
		: Layout(layout)
		, VertexCount(vertexCount)
	{
#define AccumulateStride(Usage) if (layout & Usage) { Stride += usageStride(Usage); }
		AccumulateStride(eRVertexUsage::ePosition);
		AccumulateStride(eRVertexUsage::eNormal);
		AccumulateStride(eRVertexUsage::eBiNormal);
		AccumulateStride(eRVertexUsage::eTangent);
		AccumulateStride(eRVertexUsage::eBiTangent);
		AccumulateStride(eRVertexUsage::eTexcoord0);
		AccumulateStride(eRVertexUsage::eColor0);
#undef AccumulateStride

		Data = std::make_unique<byte8_t>(Stride * vertexCount);
	}

	std::string desc()
	{
		return std::string();
	}

	constexpr size_t usageStride(eRVertexUsage usage) const
	{
		switch (usage)
		{
		case ePosition:
		case eNormal:
		case eBiNormal:
		case eTangent:
		case eBiTangent:
			return sizeof(Vec3);
		case eTexcoord0:
			return sizeof(Vec2);
		case eColor0:
			return sizeof(Vec4);
		}

		return 0u;
	}

	size_t Stride = 0u;

	template<eRVertexUsage usage, class Vector = Vec3>
	void set(const Vector& value, uint32_t index)
	{
		assert(sizeof(Vector) == usageStride(usage));
		assert(index < VertexCount);

		byte8_t* dst = Data.get()[index * Stride + offset(usage)];
		memcpy_s(dst, sizeof(Vector), &value, sizeof(Vector));
	}

	template<eRVertexUsage usage, class Vector = Vec3>
	Vector get(uint32_t index)
	{
		assert(sizeof(Vector) == usageStride(usage));
		assert(index < VertexCount);

		return Vector();
	}
protected:
	std::unique_ptr<byte8_t> Data = nullptr;
	uint32_t VertexCount = 0u;
	uint32_t Layout = 0u;

	inline size_t offset(eRVertexUsage usage) const
	{
		return 0u;
	}
};
#endif

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
		enum eTextureType /// based on assimp
		{
			eNone,
			eDiffuse,
			eSpecular,
			eAmbient,
			eMissive,
			eHeight,
			eNormal,
			eShiniess,
			eOpacity,
			eDisplacement,
			eLightMap,
			eReflection,
			eBaseColor,
			eNormalCamera,
			eEmissionColor,
			eMetalness,
			eDiffuseRoughness,
			eAmbientOcclusion,
			eUnknown
		};

		eTextureType Type = eUnknown;
		uint32_t Index = 0u;
		std::string Name;
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

	void draw(const Camera& camera, const GfxViewport& viewport, bool8_t wireframe = false);
	void draw(GfxPipelineState* state);
	
	inline AABB boundingBox() const
	{
		return m_BoundingBox;
	}
protected:
	friend class AssetTool::AssetDatabase;

	void initPipelineState();
private:
	bool8_t m_Valid = false;
	AABB m_BoundingBox;
	uint32_t m_BoundingBoxVertexCount = 0u;
	std::vector<GfxMesh> m_Meshes;
	std::array<std::vector<GfxTexturePtr>, GfxTexture::eUnknown> m_Textures; /// aiTextureType_UNKNOWN
	GfxGpuBufferPtr m_UniformBuffer;
	static GfxPipelineState s_PipelineState;
};

#endif
