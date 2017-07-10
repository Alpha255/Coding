#include "SDKMesh.h"
#include "../D3DUtil/D3DGraphic.h"
#include <fstream>
#include <memory>
///#include <wrl/client.h>

extern D3DGraphic* g_Renderer;

SDKMesh::~SDKMesh()
{

}

uint32_t SDKMesh::GetInputLayoutDesc(_In_reads_(32) const D3DVERTEXELEMENT9 decl[], std::vector<D3D11_INPUT_ELEMENT_DESC>& inputDesc)
{
	static const D3D11_INPUT_ELEMENT_DESC s_Elements[] =
	{
		{ "SV_Position",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",        0, DXGI_FORMAT_B8G8R8A8_UNORM,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",  0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	uint32_t offset = 0U;
	uint32_t texcoords = 0U;
	uint32_t flags = 0U;
	bool posFound = false;

	for (uint32_t index = 0; index < s_MaxVertexElements; ++index)
	{
		if (decl[index].Usage == 0xFF)
		{
			break;
		}

		if (decl[index].Type == eUnUsed)
		{
			break;
		}

		if (decl[index].Offset != offset)
		{
			break;
		}

		if (decl[index].Usage == D3DDECLUSAGE_POSITION)
		{
			if (decl[index].Type == eFloat3)
			{
				inputDesc.push_back(s_Elements[0]);
				offset += 12;
				posFound = true;
			}
			else
			{
				break;
			}
		}
		else if (decl[index].Usage == D3DDECLUSAGE_NORMAL || decl[index].Usage == D3DDECLUSAGE_TANGENT || decl[index].Usage == D3DDECLUSAGE_BINORMAL)
		{
			size_t base = 1;
			if (decl[index].Usage == D3DDECLUSAGE_TANGENT)
			{
				base = 3;
			}
			else if (decl[index].Usage == D3DDECLUSAGE_BINORMAL)
			{
				base = 4;
			}

			D3D11_INPUT_ELEMENT_DESC desc = s_Elements[base];

			bool unk = false;
			switch (decl[index].Type)
			{
			case eFloat3:                 
				assert(desc.Format == DXGI_FORMAT_R32G32B32_FLOAT); 
				offset += 12; 
				break;
			case eUByte4N:                
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
				flags |= Material::eBiasedVertexNormals; 
				offset += 4; 
				break;
			case eShort4N:                
				desc.Format = DXGI_FORMAT_R16G16B16A16_SNORM; 
				offset += 8; 
				break;
			case eFloat16_4:              
				desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; 
				offset += 8; 
				break;
			case eDXGI_R10G10B10A2_UNorm:
				desc.Format = DXGI_FORMAT_R10G10B10A2_UNORM; 
				flags |= Material::eBiasedVertexNormals; 
				offset += 4; 
				break;
			case eDXGI_R11G11B10_Float:
				desc.Format = DXGI_FORMAT_R11G11B10_FLOAT; 
				flags |= Material::eBiasedVertexNormals; 
				offset += 4; 
				break;
			case eDXGI_R8G8B8A8_SNorm:
				desc.Format = DXGI_FORMAT_R8G8B8A8_SNORM; 
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

			if (decl[index].Usage == D3DDECLUSAGE_TANGENT)
			{
				flags |= Material::eNormalMaps;
			}

			inputDesc.push_back(desc);
		}
		else if (decl[index].Usage == D3DDECLUSAGE_COLOR)
		{
			D3D11_INPUT_ELEMENT_DESC desc = s_Elements[2];

			bool unk = false;
			switch (decl[index].Type)
			{
			case eFloat4:                 
				desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; 
				offset += 16; 
				break;
			case eColor:               
				assert(desc.Format == DXGI_FORMAT_B8G8R8A8_UNORM); 
				offset += 4; 
				break;
			case eUByte4N:                
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
				offset += 4; 
				break;
			case eFloat16_4:              
				desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; 
				offset += 8; 
				break;
			case eDXGI_R10G10B10A2_UNorm: 
				desc.Format = DXGI_FORMAT_R10G10B10A2_UNORM; 
				offset += 4; 
				break;
			case eDXGI_R11G11B10_Float:
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

			flags |= Material::ePerVertexClr;

			inputDesc.push_back(desc);
		}
		else if (decl[index].Usage == D3DDECLUSAGE_TEXCOORD)
		{
			D3D11_INPUT_ELEMENT_DESC desc = s_Elements[5];
			desc.SemanticIndex = decl[index].UsageIndex;

			bool unk = false;
			switch (decl[index].Type)
			{
			case eFloat1:    
				desc.Format = DXGI_FORMAT_R32_FLOAT; 
				offset += 4; 
				break;
			case eFloat2:    
				assert(desc.Format == DXGI_FORMAT_R32G32_FLOAT); 
				offset += 8; 
				break;
			case eFloat3:    
				desc.Format = DXGI_FORMAT_R32G32B32_FLOAT; 
				offset += 12; 
				break;
			case eFloat4:    
				desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; 
				offset += 16; 
				break;
			case eFloat16_2: 
				desc.Format = DXGI_FORMAT_R16G16_FLOAT; 
				offset += 4; 
				break;
			case eFloat16_4: 
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

			++texcoords;

			inputDesc.push_back(desc);
		}
		else if (decl[index].Usage == D3DDECLUSAGE_BLENDINDICES)
		{
			if (decl[index].Type == eUByte4)
			{
				flags |= Material::eSkinning;
				inputDesc.push_back(s_Elements[6]);
				offset += 4;
			}
			else
			{
				break;
			}
		}
		else if (decl[index].Usage == D3DDECLUSAGE_BLENDWEIGHT)
		{
			if (decl[index].Type == eUByte4N)
			{
				flags |= Material::eSkinning;
				inputDesc.push_back(s_Elements[7]);
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

	assert(posFound);

	if (texcoords == 2)
	{
		flags |= Material::eDualTexture;
	}

	return flags;
}

bool SDKMesh::IsValid(const uint8_t* const pMeshData, size_t meshDataSize)
{
	const Header* pHeader = reinterpret_cast<const Header*>(pMeshData);
	size_t meshHeaderSize = sizeof(Header) +
		pHeader->NumVertexBuffers * sizeof(VertexBufferHeader) +
		pHeader->NumIndexBuffers * sizeof(IndexBufferHeader);

	bool bVaild = true;

	if (meshHeaderSize != pHeader->HeaderSize ||
		meshDataSize < pHeader->HeaderSize ||
		s_Version != pHeader->Version ||
		pHeader->IsBigEndian ||
		!pHeader->NumMeshes ||
		!pHeader->NumVertexBuffers ||
		!pHeader->NumIndexBuffers ||
		!pHeader->NumTotalSubsets ||
		!pHeader->NumMaterials ||
		meshDataSize < pHeader->VertexStreamHeadersOffset ||
		meshDataSize < (pHeader->VertexStreamHeadersOffset + pHeader->NumVertexBuffers * sizeof(VertexBufferHeader)) ||
		meshDataSize < pHeader->IndexStreamHeadersOffset ||
		meshDataSize < (pHeader->IndexStreamHeadersOffset + pHeader->NumIndexBuffers * sizeof(IndexBufferHeader)) ||
		meshDataSize < pHeader->MeshDataOffset || 
		meshDataSize < (pHeader->MeshDataOffset + pHeader->NumMeshes * sizeof(Mesh)) ||
		meshDataSize < pHeader->SubsetDataOffset ||
		meshDataSize < (pHeader->SubsetDataOffset + pHeader->NumTotalSubsets * sizeof(SubSet)) || 
		meshDataSize < pHeader->FrameDataOffset ||
		meshDataSize < (pHeader->FrameDataOffset + pHeader->NumFrames * sizeof(Frame)) || 
		meshDataSize < pHeader->MaterialDataOffset ||
		meshDataSize < (pHeader->MaterialDataOffset + pHeader->NumMaterials * sizeof(Material)))
	{
		bVaild = false;
	}

	uint64_t bufferDataOffset = pHeader->HeaderSize + pHeader->NonBufferDataSize;
	if (meshDataSize < bufferDataOffset ||
		meshDataSize < bufferDataOffset + pHeader->BufferDataSize)
	{
		bVaild = false;
	}

	return bVaild;
}

void SDKMesh::LoadFromFile(const char* pFileName)
{
	assert(pFileName);

	std::ifstream meshFile(ResourceFilePath(pFileName, D3DGraphic::eRTSDKMesh), std::ios::in);
	assert(meshFile.good());

	meshFile.seekg(0U, std::ios::end);
	size_t fileSize = meshFile.tellg();
	assert(fileSize >= sizeof(Header));

	meshFile.seekg(0U, std::ios::beg);
	uint8_t* pMeshData = new uint8_t[fileSize]();
	meshFile.read((char*)pMeshData, fileSize);

	assert(IsValid(pMeshData, fileSize));

	LoadFromMemory(pMeshData, fileSize);
}

void SDKMesh::LoadFromMemory(const uint8_t* const pMeshData, size_t meshDataSize)
{
	const Header* pHeader = reinterpret_cast<const Header*>(pMeshData);
	assert(pHeader);

	const VertexBufferHeader* pVBHeader = reinterpret_cast<const VertexBufferHeader*>(pMeshData + pHeader->VertexStreamHeadersOffset);
	assert(pVBHeader);

	const IndexBufferHeader* pIBHeader = reinterpret_cast<const IndexBufferHeader*>(pMeshData + pHeader->IndexStreamHeadersOffset);
	assert(pIBHeader);

	const Mesh* pMesh = reinterpret_cast<const Mesh*>(pMeshData + pHeader->MeshDataOffset);
	assert(pMesh);

	const SubSet* pSubSet = reinterpret_cast<const SubSet*>(pMeshData + pHeader->SubsetDataOffset);
	assert(pSubSet);

	const Frame* pFrame = reinterpret_cast<const Frame*>(pMeshData + pHeader->FrameDataOffset);
	assert(pFrame);

	const Material* pMaterial = reinterpret_cast<const Material*>(pMeshData + pHeader->MaterialDataOffset);
	assert(pMaterial);

	uint64_t bufferDataOffset = pHeader->HeaderSize + pHeader->NonBufferDataSize;
	const uint8_t* pBufferData = pMeshData + bufferDataOffset;
	assert(pBufferData);

	///std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer> >;
	/// Create vertex buffers
	std::vector<Ref<ID3D11Buffer> > vertexBuffers(pHeader->NumVertexBuffers);

	std::vector<std::shared_ptr<std::vector<D3D11_INPUT_ELEMENT_DESC>>> vertexDecls(pHeader->NumVertexBuffers);

	std::vector<uint32_t> materialFlags(pHeader->NumVertexBuffers);

	for (uint32_t i = 0U; i < pHeader->NumVertexBuffers; ++i)
	{
		assert((meshDataSize >= pVBHeader[i].DataOffset) && (meshDataSize >= (pVBHeader[i].DataOffset + pVBHeader[i].SizeBytes)));

		vertexDecls[i] = std::make_shared<std::vector<D3D11_INPUT_ELEMENT_DESC>>();

		uint32_t flags = GetInputLayoutDesc(pVBHeader[i].Decl, *vertexDecls[i].get());

		if (flags & Material::eSkinning)
		{
			flags &= ~(Material::eDualTexture | Material::eNormalMaps);
		}
		if (flags & Material::eDualTexture)
		{
			flags &= ~Material::eNormalMaps;
		}

		materialFlags[i] = flags;

		const uint8_t* pVertices = reinterpret_cast<const uint8_t*>(pBufferData + (pVBHeader[i].DataOffset - bufferDataOffset));
		assert(pVertices);

		g_Renderer->CreateVertexBuffer(vertexBuffers[i].GetReference(), (uint32_t)pVBHeader[i].SizeBytes, D3D11_USAGE_DEFAULT, (void*)pVertices);
	}

	/// Create index buffers
	std::vector<Ref<ID3D11Buffer>> indexBuffers(pHeader->NumIndexBuffers);

	for (uint32_t i = 0; i < pHeader->NumIndexBuffers; ++i)
	{
		assert((meshDataSize >= pIBHeader[i].DataOffset) && 
			(meshDataSize >= (pIBHeader[i].DataOffset + pIBHeader[i].SizeBytes)) &&
		((pIBHeader[i].IndexType == eIT_16BIT || pIBHeader[i].IndexType == eIT_32BIT)));

		const uint8_t* pIndices = reinterpret_cast<const uint8_t*>(pBufferData + (pIBHeader[i].DataOffset - bufferDataOffset));
		assert(pIndices);

		g_Renderer->CreateIndexBuffer(indexBuffers[i].GetReference(), (uint32_t)pIBHeader[i].SizeBytes, D3D11_USAGE_DEFAULT, (void*)pIndices);
	}

	/// Create meshes

}

