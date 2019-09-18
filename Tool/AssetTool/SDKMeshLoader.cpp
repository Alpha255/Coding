#include "AssetTool.h"
#include "Base/AssetFile.h"
#include "Colorful/Public/Model.h"
#include <dxgiformat.h>
#include <DirectXTK/Inc/Effects.h>
#include <DirectXTK/Inc/Model.h>
#include <DirectXTK/Src/SDKMesh.h>
#include <DirectXTK/Src/Shaders/Compiled/BasicEffect_VSBasic.inc>
#include <DirectXTK/Src/Shaders/Compiled/BasicEffect_VSBasicNoFog.inc>
#include <DirectXTK/Src/Shaders/Compiled/BasicEffect_VSBasicVcNoFog.inc>
#include <DirectXTK/Src/Shaders/Compiled/BasicEffect_VSBasicTxNoFog.inc>
#include <DirectXTK/Src/Shaders/Compiled/BasicEffect_VSBasicTxVcNoFog.inc>
#include <DirectXTK/Src/Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTx.inc>
#include <DirectXTK/Src/Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxVc.inc>

NamespaceBegin(AssetTool)

enum eLayoutType
{
	ePos     = 0x00000001,
	eNormal  = 0x00000010,
	eUV      = 0x00000100,
	eTangent = 0x00001000
};

bool IsValid(size_t size, const DXUT::SDKMESH_HEADER *pHeader)
{
	if (!pHeader || size < sizeof(DXUT::SDKMESH_HEADER) || size < pHeader->HeaderSize)
	{
		return false;
	}

	size_t headerSize = sizeof(DXUT::SDKMESH_HEADER)
		+ pHeader->NumVertexBuffers * sizeof(DXUT::SDKMESH_VERTEX_BUFFER_HEADER)
		+ pHeader->NumIndexBuffers * sizeof(DXUT::SDKMESH_INDEX_BUFFER_HEADER);
	if (pHeader->HeaderSize != headerSize)
	{
		return false;
	}

	if (pHeader->Version != DXUT::SDKMESH_FILE_VERSION ||
		pHeader->IsBigEndian || 
		!pHeader->NumMeshes || 
		!pHeader->NumVertexBuffers || 
		!pHeader->NumIndexBuffers || 
		!pHeader->NumTotalSubsets || 
		!pHeader->NumMaterials)
	{
		return false;
	}

	if (size < pHeader->VertexStreamHeadersOffset
		|| (size < (pHeader->VertexStreamHeadersOffset + uint64_t(pHeader->NumVertexBuffers) * sizeof(DXUT::SDKMESH_VERTEX_BUFFER_HEADER))))
	{
		return false;
	}

	if (size < pHeader->IndexStreamHeadersOffset
		|| (size < (pHeader->IndexStreamHeadersOffset + uint64_t(pHeader->NumIndexBuffers) * sizeof(DXUT::SDKMESH_INDEX_BUFFER_HEADER))))
	{
		return false;
	}

	if (size < pHeader->MeshDataOffset
		|| (size < (pHeader->MeshDataOffset + uint64_t(pHeader->NumMeshes) * sizeof(DXUT::SDKMESH_MESH))))
	{
		return false;
	}

	if (size < pHeader->SubsetDataOffset
		|| (size < (pHeader->SubsetDataOffset + uint64_t(pHeader->NumTotalSubsets) * sizeof(DXUT::SDKMESH_SUBSET))))
	{
		return false;
	}

	if (size < pHeader->FrameDataOffset
		|| (size < (pHeader->FrameDataOffset + uint64_t(pHeader->NumFrames) * sizeof(DXUT::SDKMESH_FRAME))))
	{
		return false;
	}

	if (size < pHeader->MaterialDataOffset
		|| (size < (pHeader->MaterialDataOffset + uint64_t(pHeader->NumMaterials) * sizeof(DXUT::SDKMESH_MATERIAL))))
	{
		return false;
	}

	uint64_t bufferDataOffset = pHeader->HeaderSize + pHeader->NonBufferDataSize;
	if ((size < bufferDataOffset)
		|| (size < bufferDataOffset + pHeader->BufferDataSize))
	{
		return false;
	}

	return true;
}

inline uint32_t GetVertexStride(uint32_t format)
{
	switch (format)
	{
	case eRGBA32_Float:
		return (uint32_t)(sizeof(float) * 4U);
	case eRGB32_Float:
		return (uint32_t)(sizeof(float) * 3U);
	case eBGRA8_UNorm:
	case eRGBA8_UInt:
	case eRGBA8_UNorm:
		return (uint32_t)(sizeof(uint32_t));
	case eRG32_Float:
		return (uint32_t)(sizeof(float) * 2U);
	case eRGB10A2_UNorm:
	case eRG11B10_Float:
	case eR32_Float:
		return (uint32_t)(sizeof(float));
	case eRG16_Float:
		return (uint32_t)(sizeof(uint16_t) * 2U);
	case eRGBA16_SNorm:
	case eRGBA16_Float:
		return (uint32_t)(sizeof(uint16_t) * 3U);
	}

	assert(0);

	return 0U;
}

RVertexShader GetVertexShader(const std::vector<Geometry::VertexLayout> &layouts)
{
	RVertexShader shader;

	uint32_t flags = 0U;
	for (uint32_t i = 0U; i < layouts.size(); ++i)
	{
		if (layouts[i].Semantic == "POSITION")
		{
			flags |= ePos;
		}
		else if (layouts[i].Semantic == "NORMAL")
		{
			flags |= eNormal;
		}
		else if (layouts[i].Semantic == "TANGENT")
		{
			flags |= eTangent;
		}
		else if (layouts[i].Semantic == "TEXCOORD")
		{
			flags |= eUV;
		}
	}

	switch (flags)
	{
	case ePos:
		shader.Create("SDKMesh.shader", "VSMain_Pos");
		break;
	case ePos | eNormal:
		shader.Create("SDKMesh.shader", "VSMain_PosNormal");
		break;
	case ePos | eUV:
		shader.Create("SDKMesh.shader", "VSMain_PosUV");
		break;
	case ePos | eNormal | eUV:
		shader.Create("SDKMesh.shader", "VSMain_PosNormalUV");
		break;
	case ePos | eNormal | eTangent | eUV:
		shader.Create("SDKMesh.shader", "VSMain_PosNormalTangentUV");
		break;
	default:
		assert(0);
		break;
	}

	return shader;
}

bool CompareVertexLayout(const std::vector<Geometry::VertexLayout> &left, const std::vector<Geometry::VertexLayout> &right)
{
	if (left.size() != right.size())
	{
		return false;
	}

	bool bSame = true;
	for (uint32_t i = 0U; i < left.size(); ++i)
	{
		if (left[i].Format != right[i].Format)
		{
			bSame = false;
			break;
		}
	}

	return bSame;
}

void GetVertexLayouts(_In_reads_(32) const DXUT::D3DVERTEXELEMENT9 decl[], std::vector<Geometry::VertexLayout>& vertexLayouts)
{
	static_assert(D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT >= 32, "SDKMESH supports decls up to 32 entries");

	static const std::vector<Geometry::VertexLayout> s_VertexLayouts =
	{
		{ "POSITION",  0U, 0U, eRGB32_Float },
		{ "NORMAL",       0U, 0U, eRGB32_Float },
		{ "COLOR",        0U, 0U, eRGBA8_UNorm },
		{ "TANGENT",      0U, 0U, eRGB32_Float },
		{ "BINNORMAl",    0U, 0U, eRGB32_Float },
		{ "TEXCOORD",     0U, 0U, eRG32_Float  },
		{ "BLENDINDICES", 0U, 0U, eRGBA8_UInt  },
		{ "BLENDWEIGHT",  0U, 0U, eRGBA8_UNorm },
	};

	uint32_t offset = 0U;

	bool posfound = false;

	for (uint32_t index = 0; index < DXUT::MAX_VERTEX_ELEMENTS; ++index)
	{
		if (decl[index].Usage == 0xFF)
		{
			break;
		}

		if (decl[index].Type == DXUT::D3DDECLTYPE_UNUSED)
		{
			break;
		}

		if (decl[index].Offset != offset)
		{
			break;
		}

		if (decl[index].Usage == DXUT::D3DDECLUSAGE_POSITION)
		{
			if (decl[index].Type == DXUT::D3DDECLTYPE_FLOAT3)
			{
				Geometry::VertexLayout layout = s_VertexLayouts[0];
				layout.Stride = GetVertexStride(layout.Format);
				vertexLayouts.emplace_back(layout);
				offset += layout.Stride;
				posfound = true;
			}
			else
			{
				break;
			}
		}
		else if (decl[index].Usage == DXUT::D3DDECLUSAGE_NORMAL
			|| decl[index].Usage == DXUT::D3DDECLUSAGE_TANGENT
			|| decl[index].Usage == DXUT::D3DDECLUSAGE_BINORMAL)
		{
			size_t base = 1;
			if (decl[index].Usage == DXUT::D3DDECLUSAGE_TANGENT)
			{
				base = 3;
			}
			else if (decl[index].Usage == DXUT::D3DDECLUSAGE_BINORMAL)
			{
				base = 4;
			}

			Geometry::VertexLayout layout = s_VertexLayouts[base];

			bool unk = false;
			switch (decl[index].Type)
			{
			case DXUT::D3DDECLTYPE_FLOAT3:
				assert(layout.Format == eRGB32_Float);
				break;
			case DXUT::D3DDECLTYPE_UBYTE4N:
				layout.Format = eRGBA8_UNorm;
				break;
			case DXUT::D3DDECLTYPE_SHORT4N:
				layout.Format = eRGBA16_SNorm;
				break;
			case DXUT::D3DDECLTYPE_FLOAT16_4:
				layout.Format = eRGBA16_Float;
				break;
			case DXUT::D3DDECLTYPE_DXGI_R10G10B10A2_UNORM:
				layout.Format = eRGB10A2_UNorm;
				break;
			case DXUT::D3DDECLTYPE_DXGI_R11G11B10_FLOAT:
				layout.Format = eRG11B10_Float;
				break;
			case DXUT::D3DDECLTYPE_DXGI_R8G8B8A8_SNORM:
				layout.Format = eRGBA8_SNorm;
				break;
			case DXUT::D3DDECLTYPE_DEC3N:
				layout.Format = eRGB10A2_UNorm;
				break;
			default:
				unk = true;
				break;
			}

			if (unk)
			{
				break;
			}

			layout.Stride = GetVertexStride(layout.Format);
			offset += layout.Stride;
			vertexLayouts.emplace_back(layout);
		}
		else if (decl[index].Usage == DXUT::D3DDECLUSAGE_COLOR)
		{
			Geometry::VertexLayout layout = s_VertexLayouts[2];

			bool unk = false;
			switch (decl[index].Type)
			{
			case DXUT::D3DDECLTYPE_FLOAT4:
				layout.Format = eRGBA32_Float;
				break;
			case DXUT::D3DDECLTYPE_D3DCOLOR:
				assert(layout.Format == eBGRA8_UNorm);
				break;
			case DXUT::D3DDECLTYPE_UBYTE4N:
				layout.Format = eRGBA8_UNorm;
				break;
			case DXUT::D3DDECLTYPE_FLOAT16_4:
				layout.Format = eRGBA16_Float;
				break;
			case DXUT::D3DDECLTYPE_DXGI_R10G10B10A2_UNORM:
				layout.Format = eRGB10A2_UNorm;
				break;
			case DXUT::D3DDECLTYPE_DXGI_R11G11B10_FLOAT:
				layout.Format = eRG11B10_Float;
				break;

			default:
				unk = true;
				break;
			}

			if (unk)
			{
				break;
			}

			layout.Stride = GetVertexStride(layout.Format);
			offset += layout.Stride;
			vertexLayouts.emplace_back(layout);
		}
		else if (decl[index].Usage == DXUT::D3DDECLUSAGE_TEXCOORD)
		{
			Geometry::VertexLayout layout = s_VertexLayouts[5];
			assert(decl[index].UsageIndex == 0U);

			bool unk = false;
			switch (decl[index].Type)
			{
			case DXUT::D3DDECLTYPE_FLOAT1:
				layout.Format = eR32_Float;
				break;
			case DXUT::D3DDECLTYPE_FLOAT2:
				assert(layout.Format == eRG32_Float);
				break;
			case DXUT::D3DDECLTYPE_FLOAT3:
				layout.Format = eRGB32_Float;
				break;
			case DXUT::D3DDECLTYPE_FLOAT4:
				layout.Format = eRGBA32_Float;
				break;
			case DXUT::D3DDECLTYPE_FLOAT16_2:
				layout.Format = eRG16_Float;
				break;
			case DXUT::D3DDECLTYPE_FLOAT16_4:
				layout.Format = eRGBA16_Float;
				break;

			default:
				unk = true;
				break;
			}

			if (unk)
			{
				break;
			}

			layout.Stride = GetVertexStride(layout.Format);
			offset += layout.Stride;
			vertexLayouts.emplace_back(layout);
		}
		else if (decl[index].Usage == DXUT::D3DDECLUSAGE_BLENDINDICES)
		{
			if (decl[index].Type == DXUT::D3DDECLTYPE_UBYTE4)
			{
				Geometry::VertexLayout layout = s_VertexLayouts[6];
				layout.Stride = GetVertexStride(layout.Format);
				vertexLayouts.emplace_back(layout);
				offset += layout.Stride;
			}
			else
			{
				break;
			}
		}
		else if (decl[index].Usage == DXUT::D3DDECLUSAGE_BLENDWEIGHT)
		{
			if (decl[index].Type == DXUT::D3DDECLTYPE_UBYTE4N)
			{
				Geometry::VertexLayout layout = s_VertexLayouts[7];
				layout.Stride = GetVertexStride(layout.Format);
				vertexLayouts.emplace_back(layout);
				offset += layout.Stride;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	assert(posfound);
}

bool LoadSDKMesh(
	AssetFile &asset,
	__out Geometry::Model &model)
{
	assert(!model.IsValid());

	auto pData = asset.Load().get();
	auto pHeader = (const DXUT::SDKMESH_HEADER *)(pData);

	assert(IsValid(asset.GetSize(), pHeader));

#if 0
	std::string path(asset.GetPath());
	std::wstring wpath(path.cbegin(), path.cend());
	DirectX::EffectFactory factory(REngine::Instance().GetDevice().Get());
	::SetCurrentDirectoryA("H:\\Coding\\Assets\\SDKMeshs\\SquidRoom");
	model.pModel = DirectX::Model::CreateFromSDKMESH(REngine::Instance().GetDevice().Get(), wpath.c_str(), factory);
#endif

	/// Vertex Buffer
	std::vector<uint32_t> layoutIndex(pHeader->NumVertexBuffers);
	std::vector<std::vector<Geometry::VertexLayout>> vertexLayouts;
	auto pVertexBufferHeaders = reinterpret_cast<const DXUT::SDKMESH_VERTEX_BUFFER_HEADER*>(pData + pHeader->VertexStreamHeadersOffset);
	for (uint32_t i = 0U; i < pHeader->NumVertexBuffers; ++i)
	{
		auto &vbHeader = pVertexBufferHeaders[i];

		assert(vbHeader.SizeBytes <= (D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024U * 1024U));
		assert(asset.GetSize() >= vbHeader.DataOffset && asset.GetSize() >= (vbHeader.DataOffset + vbHeader.SizeBytes));

		std::vector<Geometry::VertexLayout> vertexLayout;
		GetVertexLayouts(vbHeader.Decl, vertexLayout);

		uint32_t index = 0U;
		for (; index < vertexLayouts.size(); ++index)
		{
			if (CompareVertexLayout(vertexLayouts[index], vertexLayout))
			{
				break;
			}
		}
		if (index == vertexLayouts.size())
		{
			vertexLayouts.emplace_back(vertexLayout);
		}
		layoutIndex[i] = index;

		RBuffer vertexBuffer;
		vertexBuffer.CreateAsVertexBuffer(vbHeader.SizeBytes, eGpuReadOnly, pData + vbHeader.DataOffset);

		model.AddBuffer(vertexBuffer, Geometry::Model::eVertexBuffer);
	}
	assert(vertexLayouts.size() == 1U);

	/// Index Buffer
	auto pIndexBufferHeaders = reinterpret_cast<const DXUT::SDKMESH_INDEX_BUFFER_HEADER*>(pData + pHeader->IndexStreamHeadersOffset);
	for (uint32_t i = 0U; i < pHeader->NumIndexBuffers; ++i)
	{
		auto &ibHeader = pIndexBufferHeaders[i];

		assert(ibHeader.SizeBytes <= (D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024U * 1024U));
		assert(asset.GetSize() >= ibHeader.DataOffset && asset.GetSize() >= (ibHeader.DataOffset + ibHeader.SizeBytes));

		RBuffer indexBuffer;
		indexBuffer.CreateAsIndexBuffer(ibHeader.SizeBytes, eGpuReadOnly, pData + ibHeader.DataOffset);

		model.AddBuffer(indexBuffer, Geometry::Model::eIndexBuffer);
	}

	/// Subsets
	auto pSDKMesh = reinterpret_cast<const DXUT::SDKMESH_MESH*>(pData + pHeader->MeshDataOffset);
	auto pSubsets = reinterpret_cast<const DXUT::SDKMESH_SUBSET*>(pData + pHeader->SubsetDataOffset);
	std::vector<Material> materials(pHeader->NumMaterials);

	for (uint32_t i = 0U; i < pHeader->NumMeshes; ++i)
	{
		auto &mesh = pSDKMesh[i];

		assert(mesh.NumSubsets && mesh.NumVertexBuffers && mesh.IndexBuffer < pHeader->NumIndexBuffers && mesh.VertexBuffers[0] < pHeader->NumVertexBuffers);
		assert(asset.GetSize() >= mesh.SubsetOffset && (asset.GetSize() >= (mesh.SubsetOffset + sizeof(uint32_t) * mesh.NumSubsets)));
		if (mesh.NumFrameInfluences > 0U)
		{
			assert(asset.GetSize() >= mesh.FrameInfluenceOffset && (asset.GetSize() >= (mesh.FrameInfluenceOffset + sizeof(uint32_t) * mesh.NumFrameInfluences)));
		}

		Vec3 center(mesh.BoundingBoxCenter.x, mesh.BoundingBoxCenter.y, mesh.BoundingBoxCenter.z);
		Vec3 extents(mesh.BoundingBoxExtents.x, mesh.BoundingBoxExtents.y, mesh.BoundingBoxExtents.z);
		model.UpdateBoundingBox(center - extents, center + extents);

		auto subsets = reinterpret_cast<const uint32_t*>(pData + mesh.SubsetOffset);
		for (uint32_t j = 0U; j < mesh.NumSubsets; ++j)
		{
			Geometry::Model::SubModel subModel;

			uint32_t sIndex = subsets[j];

			assert(sIndex < pHeader->NumTotalSubsets);

			auto &subset = pSubsets[sIndex];

			switch (subset.PrimitiveType)
			{
			case DXUT::PT_TRIANGLE_LIST:        
				subModel.PrimitiveType = eTriangleList;
				break;
			case DXUT::PT_TRIANGLE_STRIP:       
				subModel.PrimitiveType = eTriangleStrip;
				break;
			case DXUT::PT_LINE_LIST:            
				subModel.PrimitiveType = eLineList;
				break;
			case DXUT::PT_LINE_STRIP:           
				subModel.PrimitiveType = eLineStrip;
				break;
			case DXUT::PT_POINT_LIST:           
				subModel.PrimitiveType = ePointList;
				break;
			case DXUT::PT_TRIANGLE_LIST_ADJ:    
				subModel.PrimitiveType = eTriangleListAdj;
				break;
			case DXUT::PT_TRIANGLE_STRIP_ADJ:   
				subModel.PrimitiveType = eTriangleStripAdj;
				break;
			case DXUT::PT_LINE_LIST_ADJ:        
				subModel.PrimitiveType = eLineListAdj;
				break;
			case DXUT::PT_LINE_STRIP_ADJ:       
				subModel.PrimitiveType = eLineStripAdj;
				break;
			default:
				assert(0);
				break;
			}

			subModel.IndexCount = (uint32_t)subset.IndexCount;
			subModel.StartIndex = (uint32_t)subset.IndexStart;
			subModel.VertexOffset = (int32_t)subset.VertexStart;
			subModel.IndexFormat = (pIndexBufferHeaders[mesh.IndexBuffer].IndexType == DXUT::IT_32BIT ? eR32_UInt : eR16_UInt);
			subModel.IndexBuffer = mesh.IndexBuffer;
			subModel.VertexBuffer = mesh.VertexBuffers[0];
			subModel.InputLayout = layoutIndex[subModel.VertexBuffer];
			subModel.MaterialIndex = subset.MaterialID;

			assert(subset.MaterialID < pHeader->NumMaterials);
			auto pMaterials = reinterpret_cast<const DXUT::SDKMESH_MATERIAL*>(pData + pHeader->MaterialDataOffset);
			auto &material = pMaterials[subset.MaterialID];
			if (!materials[subset.MaterialID].Valid)
			{
				if (strlen(material.DiffuseTexture) > 0U)
				{
					materials[subset.MaterialID].Set(Material::eDiffuse, material.DiffuseTexture);
				}
				if (strlen(material.NormalTexture) > 0U)
				{
					materials[subset.MaterialID].Set(Material::eNormal, material.NormalTexture);
				}
				if (strlen(material.SpecularTexture) > 0U)
				{
					materials[subset.MaterialID].Set(Material::eSpecular, material.SpecularTexture);
				}
				materials[subset.MaterialID].Valid = true;
			}

			model.AppendSubModel(subModel, false);
		}
	}

	for (uint32_t i = 0U; i < vertexLayouts.size(); ++i)
	{
		RInputLayout layout;
		RVertexShader shader = GetVertexShader(vertexLayouts[0]);
		layout.Create(shader.GetBlob(), vertexLayouts[i]);
		model.AddInputLayout(layout);
	}
	model.AddMaterials(materials);

	return true;
}

NamespaceEnd(AssetTool)