#include "Colorful/Gfx/AssetTool/Model.h"
#include "Colorful/Gfx/AssetTool/AssetTool.h"
#include "Colorful/Gfx/AssetTool/MaterialLoader.hpp"
#include "Colorful/Gfx/GfxDevice.h"

NAMESPACE_START(Gfx)

extern GfxDevice* s_GfxDevice;

ModelPtr Model::cube(float32_t length)
{
	ModelDesc desc;
	desc.VertexUsageFlags = EVertexUsage::Position | EVertexUsage::Normal | EVertexUsage::Texcoord0;
	desc.MeshDescs.resize(1u);

	desc.FaceCount = 6u;
	desc.PrimitiveTopology = EPrimitiveTopology::TriangleList;
	desc.MeshDescs[0u].FaceCount = 6u;
	desc.Name.assign("Cube");
	desc.MeshDescs[0u].Textures.fill(~0u);
	desc.buildInputLayout();

	float32_t halfLen = 0.5f * length;

	struct Vertex
	{
		Vec3 Position;
		Vec3 Normal;
		Vec2 UV;
	};
	assert(desc.VertexStride == sizeof(Vertex));

	static std::vector<Vertex> vertices
	{
		{ { -halfLen, -halfLen, -halfLen }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ { -halfLen,  halfLen, -halfLen }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
		{ {  halfLen,  halfLen, -halfLen }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ {  halfLen, -halfLen, -halfLen }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ { -halfLen, -halfLen,  halfLen }, { 0.0f, 0.0f,  1.0f }, { 1.0f, 1.0f } },
		{ {  halfLen, -halfLen,  halfLen }, { 0.0f, 0.0f,  1.0f }, { 0.0f, 1.0f } },
		{ {  halfLen,  halfLen,  halfLen }, { 0.0f, 0.0f,  1.0f }, { 0.0f, 0.0f } },
		{ { -halfLen,  halfLen,  halfLen }, { 0.0f, 0.0f,  1.0f }, { 1.0f, 0.0f } },
		{ { -halfLen,  halfLen, -halfLen }, { 0.0f, 1.0f,  0.0f }, { 0.0f, 1.0f } },
		{ { -halfLen,  halfLen,  halfLen }, { 0.0f, 1.0f,  0.0f }, { 0.0f, 0.0f } },
		{ {  halfLen,  halfLen,  halfLen }, { 0.0f, 1.0f,  0.0f }, { 1.0f, 0.0f } },
		{ {  halfLen,  halfLen, -halfLen }, { 0.0f, 1.0f,  0.0f }, { 1.0f, 1.0f } },
		{ { -halfLen, -halfLen, -halfLen }, {-1.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ {  halfLen, -halfLen, -halfLen }, {-1.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ {  halfLen, -halfLen,  halfLen }, {-1.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
		{ { -halfLen, -halfLen,  halfLen }, {-1.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { -halfLen, -halfLen,  halfLen }, {-1.0f, 0.0f,  0.0f }, { 0.0f, 1.0f } },
		{ { -halfLen,  halfLen,  halfLen }, {-1.0f, 0.0f,  0.0f }, { 0.0f, 0.0f } },
		{ { -halfLen,  halfLen, -halfLen }, {-1.0f, 0.0f,  0.0f }, { 1.0f, 0.0f } },
		{ { -halfLen, -halfLen, -halfLen }, {-1.0f, 0.0f,  0.0f }, { 1.0f, 1.0f } },
		{ {  halfLen, -halfLen, -halfLen }, { 1.0f, 0.0f,  0.0f }, { 0.0f, 1.0f } },
		{ {  halfLen,  halfLen, -halfLen }, { 1.0f, 0.0f,  0.0f }, { 0.0f, 0.0f } },
		{ {  halfLen,  halfLen,  halfLen }, { 1.0f, 0.0f,  0.0f }, { 1.0f, 0.0f } },
		{ {  halfLen, -halfLen,  halfLen }, { 1.0f, 0.0f,  0.0f }, { 1.0f, 1.0f } },
	};
	static std::vector<uint16_t> indices
	{
		 0u,  1u,  2u,  0u,  2u,  3u, /// front
		 4u,  5u,  6u,  4u,  6u,  7u, /// back
		 8u,  9u, 10u,  8u, 10u, 11u, /// top
		12u, 13u, 14u, 12u, 14u, 15u, /// bottom
		16u, 17u, 18u, 16u, 18u, 19u, /// left
		20u, 21u, 22u, 20u, 22u, 23u  /// right
	};

	desc.MeshDescs[0].VertexBufferSize = sizeof(Vertex) * vertices.size();
	desc.MeshDescs[0].VertexBuffer = std::shared_ptr<byte8_t>(new byte8_t[desc.MeshDescs[0].VertexBufferSize]());
	VERIFY(memcpy_s(desc.MeshDescs[0].VertexBuffer.get(), desc.MeshDescs[0].VertexBufferSize, vertices.data(), desc.MeshDescs[0].VertexBufferSize) == 0);
	desc.MeshDescs[0].IndexCount = static_cast<uint32_t>(indices.size());
	desc.MeshDescs[0].IndexBufferUInt16.insert(desc.MeshDescs[0].IndexBufferUInt16.end(), indices.cbegin(), indices.cend());

	return std::make_shared<Model>(desc);
}

void ModelDesc::buildInputLayout()
{
	assert(VertexUsageFlags != 0);

	if (VertexStride == 0)
	{
		InputLayout.VertexInputs.resize(1u);
		auto& vertexInput = InputLayout.VertexInputs[0];
		uint32_t location = 0u;

		if (VertexUsageFlags & EVertexUsage::Position)
		{
			VertexStride += sizeof(Vec3);
			vertexInput.Layouts.emplace_back(VertexInputLayoutDesc::VertexLayout{
				location++,
				sizeof(Vec3),
				EFormat::RGB32_Float,
				"POSITION"
				});
		}
		if (VertexUsageFlags & EVertexUsage::Normal)
		{
			VertexStride += sizeof(Vec3);
			vertexInput.Layouts.emplace_back(VertexInputLayoutDesc::VertexLayout{
				location++,
				sizeof(Vec3),
				EFormat::RGB32_Float,
				"NORMAL"
				});
		}
		if (VertexUsageFlags & EVertexUsage::Tangent)
		{
			VertexStride += sizeof(Vec3);
			vertexInput.Layouts.emplace_back(VertexInputLayoutDesc::VertexLayout{
				location++,
				sizeof(Vec3),
				EFormat::RGB32_Float,
				"TANGENT" 
				});
		}
		if (VertexUsageFlags & EVertexUsage::BiNormal)
		{
			VertexStride += sizeof(Vec3);
			vertexInput.Layouts.emplace_back(VertexInputLayoutDesc::VertexLayout{
				location++,
				sizeof(Vec3),
				EFormat::RGB32_Float,
				"BINNORMAL"
				});
		}
		if (VertexUsageFlags & EVertexUsage::BiTangent)
		{
			VertexStride += sizeof(Vec3);
			vertexInput.Layouts.emplace_back(VertexInputLayoutDesc::VertexLayout{
				location++,
				sizeof(Vec3),
				EFormat::RGB32_Float,
				"BITANGENT"
				});
		}
		if (VertexUsageFlags & EVertexUsage::Texcoord0)
		{
			VertexStride += sizeof(Vec2);
			vertexInput.Layouts.emplace_back(VertexInputLayoutDesc::VertexLayout{
				location++,
				sizeof(Vec2),
				EFormat::RG32_Float,
				"TEXCOORD0"
				});
		}
		if (VertexUsageFlags & EVertexUsage::Texcoord1)
		{
			VertexStride += sizeof(Vec2);
			vertexInput.Layouts.emplace_back(VertexInputLayoutDesc::VertexLayout{
				location++,
				sizeof(Vec2),
				EFormat::RG32_Float,
				"TEXCOORD1"
				});
		}
		if (VertexUsageFlags & EVertexUsage::Texcoord2)
		{
			VertexStride += sizeof(Vec2);
			vertexInput.Layouts.emplace_back(VertexInputLayoutDesc::VertexLayout{
				location++,
				sizeof(Vec2),
				EFormat::RG32_Float,
				"TEXCOORD2"
				});
		}
		if (VertexUsageFlags & EVertexUsage::Texcoord3)
		{
			VertexStride += sizeof(Vec2);
			vertexInput.Layouts.emplace_back(VertexInputLayoutDesc::VertexLayout{
				location++,
				sizeof(Vec2),
				EFormat::RG32_Float,
				"TEXCOORD3"
				});
		}
		if (VertexUsageFlags & EVertexUsage::Color)
		{
			VertexStride += sizeof(Vec4);
			vertexInput.Layouts.emplace_back(VertexInputLayoutDesc::VertexLayout{
				location++,
				sizeof(Vec4),
				EFormat::RGBA32_Float,
				"COLOR"
				});
		}
		if (VertexUsageFlags & EVertexUsage::Weight)
		{
			VertexStride += sizeof(Vec3);
			vertexInput.Layouts.emplace_back(VertexInputLayoutDesc::VertexLayout{
				location++,
				sizeof(Vec3),
				EFormat::RGB32_Float,
				"WEIGHT"
				});
		}
		assert(VertexStride);
	}
}

Model::Model(ModelDesc& desc)
	: MeshAttributes(desc.VertexCount, desc.IndexCount, desc.FaceCount, desc.BoneCount, desc.BoundingBox)
{
	auto materialDesc = std::move(MaterialLoader::loadMaterial("Lambert.xml"));
	materialDesc.InputLayoutDesc = desc.InputLayout;
	auto defaultMaterial = create_shared<Material>(materialDesc);

	materialDesc = std::move(MaterialLoader::loadMaterial("StandardPBR.xml"));
	materialDesc.InputLayoutDesc = desc.InputLayout;
	auto PBRMaterial = create_shared<Material>(materialDesc);

	m_DefaultPipelineState = create_shared<GraphicsPipelineState>(
		defaultMaterial,
		RasterizationStateDesc(),
		MultisampleStateDesc(),
		DepthStencilStateDesc(),
		BlendStateDesc());

	m_PBRPipelineState = create_shared<GraphicsPipelineState>(
		defaultMaterial,
		RasterizationStateDesc(),
		MultisampleStateDesc(),
		DepthStencilStateDesc(),
		BlendStateDesc());

	setDebugName(desc.Name.c_str());

	m_Meshes.resize(desc.MeshDescs.size());
	for (uint32_t i = 0u; i < desc.MeshDescs.size(); ++i)
	{
		auto& meshDesc = desc.MeshDescs[i];
		m_Meshes[i] = std::move(Mesh(meshDesc.VertexCount, meshDesc.IndexCount, meshDesc.FaceCount, meshDesc.BoneCount, meshDesc.BoundingBox));
		
		auto& mesh = m_Meshes[i];
		mesh.m_Textures.swap(meshDesc.Textures);	

		mesh.m_VertexBuffer = s_GfxDevice->createVertexBuffer(EBufferUsage::Immutable, meshDesc.VertexBufferSize, meshDesc.VertexBuffer.get());
		mesh.m_IndexBuffer = s_GfxDevice->createIndexBuffer(
			EBufferUsage::Immutable,
			static_cast<size_t>(desc.IndexFormat) * meshDesc.IndexCount,
			desc.IndexFormat == EIndexFormat::UInt32 ? 
			static_cast<const void*>(meshDesc.IndexBufferUInt32.data()) : static_cast<const void*>(meshDesc.IndexBufferUInt16.data()));
	}

	m_Textures.resize(desc.Textures.size());
	for (uint32_t i = 0u; i < desc.Textures.size(); ++i)
	{
		m_Textures[i] = AssetTool::instance().loadTexture(desc.Textures[i].c_str(), false);
	}
}

NAMESPACE_END(Gfx)
