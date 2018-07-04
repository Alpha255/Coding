#include "D3DGeometry.h"
#include "D3DObject.h"
#include "D3DEngine.h"
#include "D3DLighting.h"
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

	assert(header.Version == eFileVersion);

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

		assert(ih.IndexType == eIndexType_16Bit || ih.IndexType == eIndexType_32Bit);

		auto indices = reinterpret_cast<const uint8_t*>(pBufferData + (ih.DataOffset - bufferDataOffset));

		m_IndexBuffers[i].CreateAsIndexBuffer(ih.SizeBytes, D3DBuffer::eGpuReadWrite, indices);
	}
}

bool SDKMesh::LoadMaterial(const SDKMeshMaterial &sdkmt, Material &mt)
{
	float alpha = 1.0f;
	if (sdkmt.Ambient.x == 0 && sdkmt.Ambient.y == 0 && sdkmt.Ambient.z == 0 && sdkmt.Ambient.w == 0
		&& sdkmt.Diffuse.x == 0 && sdkmt.Diffuse.y == 0 && sdkmt.Diffuse.z == 0 && sdkmt.Diffuse.w == 0)
	{
		/// SDKMESH material color block is uninitalized; assume defaults
		mt.Set(Material::eDiffuse, Vec4(1.0f, 1.0f, 1.0f, 0.0f));
		alpha = 1.f;
	}
	else
	{
		mt.Set(Material::eDiffuse, sdkmt.Diffuse);
		/// info.ambientColor = XMFLOAT3(mh.Ambient.x, mh.Ambient.y, mh.Ambient.z);
		/// info.emissiveColor = XMFLOAT3(mh.Emissive.x, mh.Emissive.y, mh.Emissive.z);

		if (sdkmt.Diffuse.w != 1.f && sdkmt.Diffuse.w != 0.f)
		{
			alpha = sdkmt.Diffuse.w;
		}
		else
		{
			alpha = 1.f;
		}

		if (sdkmt.Power)
		{
			mt.Set(Material::eSpecular, Vec4(sdkmt.Specular.x, sdkmt.Specular.y, sdkmt.Specular.z, sdkmt.Power));
		}
	}

	if (strlen(sdkmt.DiffuseTexture) > 0U)
	{
		mt.Set(Material::eDiffuse, sdkmt.DiffuseTexture, true);
	}
	if (strlen(sdkmt.SpecularTexture) > 0U)
	{
		mt.Set(Material::eSpecular, sdkmt.SpecularTexture, true);
	}
	if (strlen(sdkmt.NormalTexture) > 0U)
	{
		mt.Set(Material::eNormal, sdkmt.NormalTexture, true);
	}

	return alpha < 1.0f;
}

void SDKMesh::LoadMeshs(const uint8_t *pData, size_t dataSize, const SDKMeshHeader &header, bool ccw, bool alpha)
{
	assert(dataSize >= header.MeshDataOffset && dataSize >= (header.MeshDataOffset + header.NumMeshes * sizeof(SDKMeshMesh)));
	auto meshArray = reinterpret_cast<const SDKMeshMesh *>(pData + header.MeshDataOffset);

	assert(dataSize >= header.SubsetDataOffset && dataSize >= (header.SubsetDataOffset + header.NumTotalSubsets * sizeof(SDKMeshSubset)));
	auto subsetArray = reinterpret_cast<const SDKMeshSubset *>(pData + header.SubsetDataOffset);

	assert(dataSize >= header.MaterialDataOffset && dataSize >= (header.MaterialDataOffset + header.NumMaterials * sizeof(SDKMeshMaterial)));
	auto materialArray = reinterpret_cast<const SDKMeshMaterial *>(pData + header.MaterialDataOffset);

	auto vbArray = reinterpret_cast<const SDKMeshVertexBufferHeader *>(pData + header.VertexStreamHeadersOffset);
	auto ibArray = reinterpret_cast<const SDKMeshIndexBufferHeader *>(pData + header.IndexStreamHeadersOffset);

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

		auto mesh = std::make_shared<ModelMesh>();
		mesh->CCW = ccw;
		mesh->Alpha = alpha;
		mesh->MeshBoundingBox.Center = mh.BoundingBoxCenter;
		mesh->MeshBoundingBox.Extents = mh.BoundingBoxExtents;

		mesh->MeshParts.reserve(mh.NumSubsets);
		ePrimitiveTopology ptType;
		for (uint32_t j = 0U; j < mh.NumSubsets; ++j)
		{
			auto sIndex = subsets[j];
			
			assert(sIndex < header.NumTotalSubsets);

			auto &subset = subsetArray[sIndex];

			switch (subset.PrimitiveType)
			{
			case eTriangle_List:      ptType = eTriangleList;     break;
			case eTriangle_Strip:     ptType = eTriangleStrip;    break;
			case eLine_List:          ptType = eLineList;         break;
			case eLine_Strip:         ptType = eLineStrip;        break;
			case ePoint_List:         ptType = ePointList;        break;
			case eTriangle_List_Adj:  ptType = eTriangleListAdj;  break;
			case eTriangle_Strip_Adj: ptType = eTriangleStripAdj; break;
			case eLine_List_Adj:      ptType = eLineListAdj;      break;
			case eLine_Strip_Adj:     ptType = eLineStripAdj;     break;
			default:                  assert(0);                  break;
			}

			assert(subset.MaterialID < header.NumMaterials);

			auto meshPart = new ModelMesh::MeshPart();
			meshPart->Mt = new Material();
			meshPart->IsAlpha = LoadMaterial(materialArray[subset.MaterialID], *meshPart->Mt);
			meshPart->IndexCount = static_cast<uint32_t>(subset.IndexCount);
			meshPart->StartIndex = static_cast<uint32_t>(subset.IndexStart);
			meshPart->VertexOffset = static_cast<uint32_t>(subset.VertexStart);
			meshPart->VertexStride = static_cast<uint32_t>(vbArray[mh.VertexBuffers[0]].StrideBytes);
			meshPart->IndexFormat = (ibArray[mh.IndexBuffer].IndexType == eIndexType_32Bit) ? eR32_UInt : eR16_UInt;
			meshPart->PrimitiveType = ptType;
			meshPart->IndexBuffer = m_IndexBuffers[mh.IndexBuffer];
			meshPart->VertexBuffer = m_VertexBuffers[mh.VertexBuffers[0]];

			mesh->MeshParts.emplace_back(meshPart);
		}

		m_Meshs.emplace_back(mesh);
	}
}

void SDKMesh::CreateEx(const char *pFileName, bool ccw, bool alpha)
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
	LoadMeshs(pData.get(), fileSize, header, ccw, alpha);

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

	if (m_ExtVertexLayout)
	{
		assert(m_VertexLayout.IsValid());
		D3DEngine::Instance().SetInputLayout(m_VertexLayout);
	}

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

			SetupVertexIndex(part);

			SetupMaterial(part, bDisableMaterial);

			D3DEngine::Instance().DrawIndexed(part->indexCount, part->startIndex, part->vertexOffset, part->primitiveType);
		}
	}
}

void SDKMesh::DrawEx(bool bAlphaParts, bool bDisableMaterial)
{
	assert(m_Created);

	assert(m_VertexLayout.IsValid());
	D3DEngine::Instance().SetInputLayout(m_VertexLayout);

	/// Draw opaque parts
	for (auto it = m_Meshs.cbegin(); it != m_Meshs.cend(); ++it)
	{
		auto mesh = it->get();
		assert(mesh);

		for (auto it = mesh->MeshParts.cbegin(); it != mesh->MeshParts.cend(); ++it)
		{
			auto part = (*it).get();
			assert(part);

			if (part->IsAlpha != bAlphaParts)
			{
				/// Skip alpha parts when drawing opaque or skip opaque parts if drawing alpha
				continue;
			}

			D3DEngine::Instance().SetVertexBuffer(part->VertexBuffer, part->VertexStride, 0U);
			D3DEngine::Instance().SetIndexBuffer(part->IndexBuffer, part->IndexFormat, 0U);

			ApplyMaterial(bDisableMaterial ? nullptr : part->Mt);

			D3DEngine::Instance().DrawIndexed(part->IndexCount, part->StartIndex, part->VertexOffset, part->PrimitiveType);
		}
	}
}

SDKMesh::ModelMesh::MeshPart::~MeshPart()
{
	SafeDelete(Mt);
}

NamespaceEnd(Geometry)