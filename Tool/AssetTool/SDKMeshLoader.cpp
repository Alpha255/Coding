#include "AssetTool.h"
#include "Base/AssetFile.h"
#include "Colorful/Public/Model.h"
#include <dxgiformat.h>
#include <DirectXTK/Src/SDKMesh.h>

NamespaceBegin(AssetTool)

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

struct InputElementDesc
{
	enum eType
	{
		ePosition,
		eNormal,
		eColor,
		eTangent,
		eBinNormal,
		eTexcoord,
		eBlendIndices,
		eBlendWeight,
		eTypeCount
	};

	eType Type = eTypeCount;
	DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
};

void GetInputElementDesc(_In_reads_(32) const DXUT::D3DVERTEXELEMENT9 decl[], std::vector<InputElementDesc>& inputDesc)
{
	static_assert(D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT >= 32, "SDKMESH supports decls up to 32 entries");

	static const InputElementDesc s_Elements[] =
	{
		{ InputElementDesc::ePosition,     DXGI_FORMAT_R32G32B32_FLOAT },
		{ InputElementDesc::eNormal,       DXGI_FORMAT_R32G32B32_FLOAT },
		{ InputElementDesc::eColor,        DXGI_FORMAT_B8G8R8A8_UNORM  },
		{ InputElementDesc::eTangent,      DXGI_FORMAT_R32G32B32_FLOAT },
		{ InputElementDesc::eBinNormal,    DXGI_FORMAT_R32G32B32_FLOAT },
		{ InputElementDesc::eTexcoord,     DXGI_FORMAT_R32G32_FLOAT    },
		{ InputElementDesc::eBlendIndices, DXGI_FORMAT_R8G8B8A8_UINT   },
		{ InputElementDesc::eBlendWeight,  DXGI_FORMAT_R8G8B8A8_UNORM  },
	};

	uint32_t offset = 0;

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
				inputDesc.emplace_back(s_Elements[0]);
				offset += 12;
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

			InputElementDesc desc = s_Elements[base];

			bool unk = false;
			switch (decl[index].Type)
			{
			case DXUT::D3DDECLTYPE_FLOAT3:
				assert(desc.Format == DXGI_FORMAT_R32G32B32_FLOAT); 
				offset += 12; 
				break;
			case DXUT::D3DDECLTYPE_UBYTE4N:
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
				offset += 4; 
				break;
			case DXUT::D3DDECLTYPE_SHORT4N:
				desc.Format = DXGI_FORMAT_R16G16B16A16_SNORM; 
				offset += 8; 
				break;
			case DXUT::D3DDECLTYPE_FLOAT16_4:
				desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; 
				offset += 8; 
				break;
			case DXUT::D3DDECLTYPE_DXGI_R10G10B10A2_UNORM:
				desc.Format = DXGI_FORMAT_R10G10B10A2_UNORM; 
				offset += 4; 
				break;
			case DXUT::D3DDECLTYPE_DXGI_R11G11B10_FLOAT:
				desc.Format = DXGI_FORMAT_R11G11B10_FLOAT; 
				offset += 4; 
				break;
			case DXUT::D3DDECLTYPE_DXGI_R8G8B8A8_SNORM:
				desc.Format = DXGI_FORMAT_R8G8B8A8_SNORM; 
				offset += 4; 
				break;
			case DXUT::D3DDECLTYPE_DEC3N:
				desc.Format = DXGI_FORMAT_R10G10B10A2_UNORM; 
				offset += 4; 
				break;
			default:
				unk = true;
				break;
			}

			if (unk)
			{
				break;
			}

			inputDesc.emplace_back(desc);
		}
		else if (decl[index].Usage == DXUT::D3DDECLUSAGE_COLOR)
		{
			InputElementDesc desc = s_Elements[2];

			bool unk = false;
			switch (decl[index].Type)
			{
			case DXUT::D3DDECLTYPE_FLOAT4:
				desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; 
				offset += 16; 
				break;
			case DXUT::D3DDECLTYPE_D3DCOLOR:
				assert(desc.Format == DXGI_FORMAT_B8G8R8A8_UNORM); 
				offset += 4; 
				break;
			case DXUT::D3DDECLTYPE_UBYTE4N:
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
				offset += 4; 
				break;
			case DXUT::D3DDECLTYPE_FLOAT16_4:
				desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; 
				offset += 8; 
				break;
			case DXUT::D3DDECLTYPE_DXGI_R10G10B10A2_UNORM:
				desc.Format = DXGI_FORMAT_R10G10B10A2_UNORM; 
				offset += 4; 
				break;
			case DXUT::D3DDECLTYPE_DXGI_R11G11B10_FLOAT:
				desc.Format = DXGI_FORMAT_R11G11B10_FLOAT; 
				offset += 4; 
				break;

			default:
				unk = true;
				break;
			}

			if (unk)
			{
				break;
			}

			inputDesc.emplace_back(desc);
		}
		else if (decl[index].Usage == DXUT::D3DDECLUSAGE_TEXCOORD)
		{
			InputElementDesc desc = s_Elements[5];
			assert(decl[index].UsageIndex == 0U);

			bool unk = false;
			switch (decl[index].Type)
			{
			case DXUT::D3DDECLTYPE_FLOAT1:
				desc.Format = DXGI_FORMAT_R32_FLOAT; 
				offset += 4; 
				break;
			case DXUT::D3DDECLTYPE_FLOAT2:
				assert(desc.Format == DXGI_FORMAT_R32G32_FLOAT); 
				offset += 8; 
				break;
			case DXUT::D3DDECLTYPE_FLOAT3:
				desc.Format = DXGI_FORMAT_R32G32B32_FLOAT; 
				offset += 12; 
				break;
			case DXUT::D3DDECLTYPE_FLOAT4:
				desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; 
				offset += 16; 
				break;
			case DXUT::D3DDECLTYPE_FLOAT16_2:
				desc.Format = DXGI_FORMAT_R16G16_FLOAT; 
				offset += 4; 
				break;
			case DXUT::D3DDECLTYPE_FLOAT16_4:
				desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; 
				offset += 8; 
				break;

			default:
				unk = true;
				break;
			}

			if (unk)
			{
				break;
			}

			inputDesc.emplace_back(desc);
		}
		else if (decl[index].Usage == DXUT::D3DDECLUSAGE_BLENDINDICES)
		{
			if (decl[index].Type == DXUT::D3DDECLTYPE_UBYTE4)
			{
				inputDesc.emplace_back(s_Elements[6]);
				offset += 4;
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
				inputDesc.emplace_back(s_Elements[7]);
				offset += 4;
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

inline size_t GetVertexStride(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R32G32B32_FLOAT:
		return sizeof(float) * 3U;
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return sizeof(uint32_t);
	case DXGI_FORMAT_R32G32_FLOAT:
		return sizeof(float) * 2U;
	case DXGI_FORMAT_R8G8B8A8_UINT:
		return sizeof(uint32_t);
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return sizeof(uint32_t);
	}

	assert(0);

	return 0U;
}

void LoadVertexData(std::vector<Geometry::Vertex> &vertices, const std::vector<InputElementDesc> &inputDesc, const uint8_t *pData)
{
	const uint8_t *pVertexData = pData;

	for (uint32_t i = 0U; i < vertices.size(); ++i)
	{
		for (uint32_t j = 0U; j < inputDesc.size(); ++j)
		{
			switch (inputDesc[j].Type)
			{
			case InputElementDesc::ePosition:
				assert(inputDesc[j].Format == DXGI_FORMAT_R32G32B32_FLOAT);
				vertices[i].Position = Vec3((const float *)pVertexData);
				break;
			case InputElementDesc::eNormal:
				assert(inputDesc[j].Format == DXGI_FORMAT_R32G32B32_FLOAT);
				vertices[i].Normal = Vec3((const float *)pVertexData);
				break;
			case InputElementDesc::eTangent:
				assert(inputDesc[j].Format == DXGI_FORMAT_R32G32B32_FLOAT);
				vertices[i].Tangent = Vec3((const float *)pVertexData);
				break;
			case InputElementDesc::eTexcoord:
				assert(inputDesc[j].Format == DXGI_FORMAT_R32G32_FLOAT);
				vertices[i].UV = Vec2((const float *)pVertexData);
				break;
			}
			pVertexData += GetVertexStride(inputDesc[j].Format);
		}
	}
}

void LoadIndexData(std::vector<uint32_t> &indices, const uint8_t *pData)
{
	const uint16_t *pIndexData = (const uint16_t *)pData;
	for (uint32_t i = 0U; i < indices.size(); ++i)
	{
		indices[i] = (uint32_t)*pIndexData;
		++pIndexData;
	}
}

bool LoadSDKMesh(
	AssetFile &asset,
	__out Geometry::Model &model)
{
	assert(!model.IsValid());

	auto pData = asset.Load().get();
	auto pHeader = (const DXUT::SDKMESH_HEADER *)(pData);

	assert(IsValid(asset.GetSize(), pHeader));
	assert(pHeader->NumVertexBuffers == pHeader->NumIndexBuffers); ///

	/// Subsets
	auto pSDKMesh = reinterpret_cast<const DXUT::SDKMESH_MESH*>(pData + pHeader->MeshDataOffset);
	auto pSubsets = reinterpret_cast<const DXUT::SDKMESH_SUBSET*>(pData + pHeader->SubsetDataOffset);

	for (uint32_t i = 0U; i < pHeader->NumMeshes; ++i)
	{
		auto &mesh = pSDKMesh[i];

		assert(mesh.NumSubsets && mesh.NumVertexBuffers && mesh.IndexBuffer < pHeader->NumIndexBuffers && mesh.VertexBuffers[0] < pHeader->NumVertexBuffers);
		assert(asset.GetSize() >= mesh.SubsetOffset && (asset.GetSize() >= (mesh.SubsetOffset + sizeof(uint32_t) * mesh.NumSubsets)));
		if (mesh.NumFrameInfluences > 0U)
		{
			assert(asset.GetSize() >= mesh.FrameInfluenceOffset && (asset.GetSize() >= (mesh.FrameInfluenceOffset + sizeof(uint32_t) * mesh.NumFrameInfluences)));
		}

		if (pHeader->NumMeshes == 1U)
		{
			Vec3 center(mesh.BoundingBoxCenter.x, mesh.BoundingBoxCenter.y, mesh.BoundingBoxCenter.z);
			Vec3 extents(mesh.BoundingBoxExtents.x, mesh.BoundingBoxExtents.y, mesh.BoundingBoxExtents.z);
			model.SetBoundingBox(center - extents, center + extents);
		}

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

			assert(subset.MaterialID < pHeader->NumMaterials);
			auto pMaterials = reinterpret_cast<const DXUT::SDKMESH_MATERIAL*>(pData + pHeader->MaterialDataOffset);
			auto &material = pMaterials[subset.MaterialID];
			if (strlen(material.DiffuseTexture) > 0U)
			{
				subModel.Mat.Set(Material::eDiffuse, material.DiffuseTexture);
			}
			if (strlen(material.NormalTexture) > 0U)
			{
				subModel.Mat.Set(Material::eNormal, material.NormalTexture);
			}
			if (strlen(material.SpecularTexture) > 0U)
			{
				subModel.Mat.Set(Material::eSpecular, material.SpecularTexture);
			}

			subModel.IndexCount = (uint32_t)subset.IndexCount;
			subModel.StartIndex = (uint32_t)subset.IndexStart;
			subModel.VertexOffset = (int32_t)subset.VertexStart;
			subModel.IndexBuffer = mesh.IndexBuffer;
			subModel.VertexBuffer = mesh.VertexBuffers[0];
			model.AppendSubModel(subModel);
		}
	}


	/// Vertex-Index
	auto pVertexBufferHeaders = reinterpret_cast<const DXUT::SDKMESH_VERTEX_BUFFER_HEADER*>(pData + pHeader->VertexStreamHeadersOffset);
	auto pIndexBufferHeaders = reinterpret_cast<const DXUT::SDKMESH_INDEX_BUFFER_HEADER*>(pData + pHeader->IndexStreamHeadersOffset);
	for (uint32_t i = 0U; i < pHeader->NumVertexBuffers; ++i)
	{
		auto &vbHeader = pVertexBufferHeaders[i];
		auto &ibHeader = pIndexBufferHeaders[i];

		assert(vbHeader.SizeBytes <= (D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024U * 1024U));
		assert(asset.GetSize() >= vbHeader.DataOffset && asset.GetSize() >= (vbHeader.DataOffset + vbHeader.SizeBytes));

		assert(ibHeader.SizeBytes <= (D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024U * 1024U));
		assert(asset.GetSize() >= ibHeader.DataOffset && asset.GetSize() >= (ibHeader.DataOffset + ibHeader.SizeBytes));

		std::vector<InputElementDesc> inputDesc;
		GetInputElementDesc(vbHeader.Decl, inputDesc);

		size_t stride = 0U;
		for (uint32_t j = 0U; j < inputDesc.size(); ++j)
		{
			stride += GetVertexStride(inputDesc[j].Format);
		}

		assert(stride == vbHeader.StrideBytes);
		assert(vbHeader.NumVertices * stride == vbHeader.SizeBytes);

		std::vector<Geometry::Vertex> vertices(vbHeader.NumVertices);
		LoadVertexData(vertices, inputDesc, pData + vbHeader.DataOffset);
		RBuffer vertexBuffer;
		vertexBuffer.CreateAsVertexBuffer(sizeof(Geometry::Vertex) * vbHeader.NumVertices, eGpuReadOnly, vertices.data());

		RBuffer indexBuffer;
		if (ibHeader.IndexType == DXUT::IT_32BIT)
		{
			indexBuffer.CreateAsIndexBuffer(ibHeader.SizeBytes, eGpuReadOnly, pData + ibHeader.DataOffset);
		}
		else if (ibHeader.IndexType == DXUT::IT_16BIT)
		{
			std::vector<uint32_t> indices(ibHeader.NumIndices);
			LoadIndexData(indices, pData + ibHeader.DataOffset);
		}
		else
		{
			assert(0);
		}

		model.AddBuffer(vertexBuffer, indexBuffer);
	}

	return true;
}

NamespaceEnd(AssetTool)