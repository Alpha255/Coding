#include "AssetTool.h"
#include "Base/AssetFile.h"
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

bool LoadSDKMesh(
	const std::string &filePath,
	__out std::vector<Geometry::Vertex> &vertices,
	__out std::vector<uint32_t> &indices)
{
	AssetFile file(filePath);

	auto pData = file.Load().get();
	auto pHeader = (const DXUT::SDKMESH_HEADER *)(pData);

	assert(IsValid(file.GetSize(), pHeader));

	return false;
}

NamespaceEnd(AssetTool)