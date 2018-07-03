#include "D3DGeometry.h"
#include "D3DObject.h"
#include "D3DEngine.h"
#include "Camera.h"
#include "System.h"

#include <fstream>
#include <DirectXTK/Inc/Effects.h>

NamespaceBegin(Geometry)

SDKMesh::SDKMeshHeader SDKMesh::LoadHeader(const uint8_t *pData, size_t dataSize)
{
	assert(dataSize >= sizeof(SDKMeshHeader));

	SDKMeshHeader header;
	memcpy(&header, pData, sizeof(SDKMeshHeader));

	size_t headerSize = sizeof(SDKMeshHeader) 
		+ header.NumVertexBuffers * sizeof(SDKMeshVertexBufferHeader) 
		+ header.NumIndexBuffers * sizeof(SDKMeshIndexBufferHeader);
	assert(header.HeaderSize == headerSize && dataSize >= headerSize);

	assert(header.Version == eSDKMeshFileVersion);

	assert(!header.IsBigEndian);

	assert(header.NumMeshes);

	assert(header.NumVertexBuffers && header.NumIndexBuffers);

	assert(header.NumTotalSubsets);

	assert(header.NumMaterials);

	return header;
}

void SDKMesh::LoadVertexBuffers(const uint8_t *pData, size_t dataSize, const SDKMeshHeader &header)
{
	assert(dataSize >= header.VertexStreamHeadersOffset && 
		dataSize >= (header.VertexStreamHeadersOffset + header.NumVertexBuffers * sizeof(SDKMeshVertexBufferHeader)));

	uint64_t bufferDataOffset = header.HeaderSize + header.NonBufferDataSize;
	assert(dataSize >= bufferDataOffset && dataSize >= (bufferDataOffset + header.BufferDataSize));
	const uint8_t *pBufferData = pData + bufferDataOffset;

	m_VertexBuffers.resize(header.NumVertexBuffers);
	auto vbArray = reinterpret_cast<const SDKMeshVertexBufferHeader *>(pData + header.VertexStreamHeadersOffset);
	for (uint32_t i = 0U; i < header.NumVertexBuffers; ++i)
	{
		auto &vh = vbArray[i];

		assert(dataSize >= vh.DataOffset && dataSize >= (vh.DataOffset + vh.SizeBytes));

		auto verts = reinterpret_cast<const uint8_t*>(pBufferData + (vh.DataOffset - bufferDataOffset));

		m_VertexBuffers[i].CreateAsVertexBuffer(vh.SizeBytes, D3DBuffer::eGpuReadWrite, verts);
	}
}

void SDKMesh::LoadIndexBuffers(const uint8_t *pData, size_t dataSize, const SDKMeshHeader &header)
{
	assert(dataSize >= header.IndexStreamHeadersOffset &&
		dataSize >= (header.IndexStreamHeadersOffset + header.NumIndexBuffers * sizeof(SDKMeshIndexBufferHeader)));

	uint64_t bufferDataOffset = header.HeaderSize + header.NonBufferDataSize;
	assert(dataSize >= bufferDataOffset && dataSize >= (bufferDataOffset + header.BufferDataSize));
	const uint8_t *pBufferData = pData + bufferDataOffset;

	m_IndexBuffers.resize(header.NumIndexBuffers);
	auto ibArray = reinterpret_cast<const SDKMeshIndexBufferHeader *>(pData + header.IndexStreamHeadersOffset);
	for (uint32_t i = 0U; i < header.NumIndexBuffers; ++i)
	{
		auto &ih = ibArray[i];

		assert(dataSize >= ih.DataOffset && dataSize >= (ih.DataOffset + ih.SizeBytes));

		assert(ih.IndexType == eSDKMeshIndexType_16Bit || ih.IndexType == eSDKMeshIndexType_32Bit);

		auto indices = reinterpret_cast<const uint8_t*>(pBufferData + (ih.DataOffset - bufferDataOffset));

		m_IndexBuffers[i].CreateAsIndexBuffer(ih.SizeBytes, D3DBuffer::eGpuReadWrite, indices);
	}
}

void SDKMesh::LoadMeshs(const uint8_t *pData, size_t dataSize, const SDKMeshHeader &header)
{
	assert(!"Not DONE!");
	assert(dataSize >= header.MeshDataOffset && dataSize >= (header.MeshDataOffset + header.NumMeshes * sizeof(SDKMeshMesh)));

	auto meshArray = reinterpret_cast<const SDKMeshMesh *>(pData + header.MeshDataOffset);
	for (uint32_t i = 0U; i < header.NumMeshes; ++i)
	{
		auto& mh = meshArray[i];

		assert(mh.NumSubsets && mh.NumVertexBuffers && mh.IndexBuffer < header.NumIndexBuffers && mh.VertexBuffers[0] < header.NumVertexBuffers);

		assert(dataSize >= mh.SubsetOffset && dataSize >= (mh.SubsetOffset + mh.NumSubsets * sizeof(uint32_t)));

		auto subsets = reinterpret_cast<const uint32_t *>(pData + mh.SubsetOffset);

		if (mh.NumFrameInfluences > 0U)
		{
			assert(dataSize >= mh.FrameInfluenceOffset && dataSize >= (mh.FrameInfluenceOffset + mh.NumFrameInfluences * sizeof(uint32_t)));
		}
	}
}

void SDKMesh::CreateFromFile(const char *pFileName)
{
	assert(!m_Created && pFileName && System::IsStrEndwith(pFileName, ".sdkmesh"));

	std::string filePath = System::ResourceFilePath(pFileName, System::eSDKMesh);

	char oldWorkingDir[MAX_PATH] = {};
	::GetCurrentDirectoryA(MAX_PATH, oldWorkingDir);
	std::size_t pos = filePath.rfind('\\') + 1;
	std::string curWorkdingDir(filePath.begin(), filePath.begin() + pos);
	::SetCurrentDirectoryA(curWorkdingDir.c_str());

	std::string fileName(filePath.begin() + pos, filePath.end());
	std::ifstream sdkmeshFile(fileName, std::ios::in | std::ios::binary);
	assert(sdkmeshFile.good());

	sdkmeshFile.seekg(0U, std::ios::end);
	size_t fileSize = (size_t)sdkmeshFile.tellg();

	std::unique_ptr<uint8_t[]> pData;
	pData.reset(new uint8_t[fileSize]());
	sdkmeshFile.seekg(0U, std::ios::beg);
	sdkmeshFile.read((char *)pData.get(), fileSize);
	sdkmeshFile.close();

	SDKMeshHeader header = LoadHeader(pData.get(), fileSize);
	LoadVertexBuffers(pData.get(), fileSize, header);
	LoadIndexBuffers(pData.get(), fileSize, header);
	LoadMeshs(pData.get(), fileSize, header);

	::SetCurrentDirectoryA(oldWorkingDir);

	m_Created = true;
}

void SDKMesh::Create(const char *pFileName, bool bLoadVertexLayout)
{
	assert(!m_Created && pFileName && System::IsStrEndwith(pFileName, ".sdkmesh"));

	m_ExtVertexLayout = !bLoadVertexLayout;

	std::string filePath = System::ResourceFilePath(pFileName, System::eSDKMesh);
	std::wstring wfilePath(filePath.begin(), filePath.end());

	ID3D11Device *pDevice = D3DEngine::Instance().GetDevice().Get();
	DirectX::EffectFactory effect(pDevice);

	m_States = std::make_unique<DirectX::CommonStates>(pDevice);

	char oldWorkingDir[MAX_PATH] = {};
	::GetCurrentDirectoryA(MAX_PATH, oldWorkingDir);
	std::size_t pos = filePath.rfind('\\') + 1;
	std::string curWorkdingDir(filePath.begin(), filePath.begin() + pos);
	::SetCurrentDirectoryA(curWorkdingDir.c_str());

	m_Model = DirectX::Model::CreateFromSDKMESH(pDevice, wfilePath.c_str(), effect, false, false, bLoadVertexLayout);

	::SetCurrentDirectoryA(oldWorkingDir);

	m_Created = true;
}

void SDKMesh::SetupVertexIndex(const DirectX::ModelMeshPart *pModelPart)
{
	assert(pModelPart);

	if (!m_ExtVertexLayout)
	{
		ID3D11InputLayout *pLayout = nullptr;
		HRCheck(pModelPart->inputLayout.CopyTo(&pLayout));
		D3DInputLayout vertexLayout;
		vertexLayout.MakeObject(pLayout);

		D3DEngine::Instance().SetInputLayout(vertexLayout);
	}

	ID3D11Buffer *pVB = nullptr;
	HRCheck(pModelPart->vertexBuffer.CopyTo(&pVB));
	D3DVertexBuffer vertexBuffer;
	vertexBuffer.Buffer.MakeObject(pVB);
	vertexBuffer.Stride = pModelPart->vertexStride;
	vertexBuffer.Offset = 0U;
	D3DEngine::Instance().SetVertexBuffer(vertexBuffer.Buffer, vertexBuffer.Stride, vertexBuffer.Offset);

	ID3D11Buffer *pIB = nullptr;
	HRCheck(pModelPart->indexBuffer.CopyTo(&pIB));
	D3DIndexBuffer indexBuffer;
	indexBuffer.Buffer.MakeObject(pIB);
	indexBuffer.Format = pModelPart->indexFormat;
	indexBuffer.Offset = 0U;
	D3DEngine::Instance().SetIndexBuffer(indexBuffer.Buffer, indexBuffer.Format, indexBuffer.Offset);
}

void SDKMesh::SetupMaterial(const DirectX::ModelMeshPart *pModelPart, bool bDisableMaterial)
{
	if (bDisableMaterial)
	{
		return;
	}

	assert(pModelPart);

	DirectX::IEffect *pEffect = pModelPart->effect.get();
	if (pEffect)
	{
		std::vector<ID3D11ShaderResourceView *> textures = pEffect->GetTextures();
		for (uint32_t i = 0U; i < textures.size(); ++i)
		{
			D3DShaderResourceView texture;
			texture.MakeObject(textures[i]);

			D3DEngine::Instance().SetShaderResourceView(texture, i, D3DShader::ePixelShader);
		}

		if (textures.size() > 0)
		{
			D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);
		}
	}
}

void SDKMesh::Draw(bool bAlphaParts, bool bDisableMaterial)
{
	assert(m_Created);

	/// Draw opaque parts
	for (auto it = m_Model->meshes.cbegin(); it != m_Model->meshes.cend(); ++it)
	{
		auto mesh = it->get();
		assert(mesh);

		for (auto it = mesh->meshParts.cbegin(); it != mesh->meshParts.cend(); ++it)
		{
			auto part = (*it).get();
			assert(part);

			if (part->isAlpha != bAlphaParts)
			{
				/// Skip alpha parts when drawing opaque or skip opaque parts if drawing alpha
				continue;
			}

			if (m_ExtVertexLayout)
			{
				assert(m_VertexLayout.IsValid());

				D3DEngine::Instance().SetInputLayout(m_VertexLayout);
			}

			SetupVertexIndex(part);

			SetupMaterial(part, bDisableMaterial);

			D3DEngine::Instance().DrawIndexed(part->indexCount, part->startIndex, part->vertexOffset, part->primitiveType);
		}
	}
}

NamespaceEnd(Geometry)