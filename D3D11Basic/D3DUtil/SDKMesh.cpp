#include "SDKMesh.h"
#include <fstream>

SDKMesh::~SDKMesh()
{

}

bool SDKMesh::IsValid(const uint8_t* const pMeshData, size_t meshDataSize)
{
	const Header* pHeader = reinterpret_cast<const Header*>(pMeshData);
	size_t meshHeaderSize = sizeof(Header) +
		pHeader->NumVertexBuffers * sizeof(VertexBufferHeader) +
		pHeader->NumIndexBuffers * sizeof(IndexBufferHeader);

	bool bVaild = false;

	if (meshHeaderSize != pHeader->HeaderSize ||
		meshDataSize < pHeader->HeaderSize ||
		s_Version != pHeader->Version ||
		pHeader->IsBigEndian ||
		!pHeader->NumMeshes ||
		!pHeader->NumVertexBuffers ||
		!pHeader->NumIndexBuffers ||
		!pHeader->NumTotalSubsets ||
		!pHeader->NumMaterials)
	{
		bVaild = false;
	}

	return bVaild;
}

void SDKMesh::LoadFromFile(const char* filePath)
{
	assert(filePath);

	std::fstream meshFile(filePath, std::ios::in);
	assert(!meshFile.bad());

	meshFile.seekg(std::ios::end);
	size_t fileSize = meshFile.tellg();
	assert(fileSize >= sizeof(Header));

	meshFile.seekg(std::ios::beg);
	uint8_t* pMeshData = new uint8_t[fileSize]();
	meshFile.read((char*)pMeshData, fileSize);

	assert(IsValid(pMeshData, fileSize));

	LoadFromMemory(pMeshData, fileSize);
}

void SDKMesh::LoadFromMemory(const uint8_t* const pMeshData, size_t meshDataSize)
{
	const Header* pHeader = reinterpret_cast<const Header*>(pMeshData);
}

