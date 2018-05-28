#include "D3DGeometry.h"
#include "D3DObject.h"
#include "D3DEngine.h"
#include "Camera.h"
#include "System.h"

#include <fstream>
#include <DirectXTK/Inc/Effects.h>

NamespaceBegin(Geometry)

void SDKMesh::Create(const char *pFileName)
{
	assert(pFileName && System::IsStrEndwith(pFileName, ".sdkmesh"));

	std::string filePath = System::ResourceFilePath(pFileName, System::eSDKMesh);
	std::wstring wfilePath(filePath.begin(), filePath.end());

	ID3D11Device *pDevice = D3DEngine::Instance().GetDevice().Get();
	DirectX::EffectFactory effect(pDevice);

	m_States = std::make_unique<DirectX::CommonStates>(pDevice);

	char workingDir[MAX_PATH] = {};
	::GetCurrentDirectoryA(MAX_PATH, workingDir);
	::SetCurrentDirectoryA(System::ResourceFileDirectory(System::eSDKMesh).c_str());

	m_Model = DirectX::Model::CreateFromSDKMESH(pDevice, wfilePath.c_str(), effect);

	::SetCurrentDirectoryA(workingDir);

	m_Created = true;
}

void SDKMesh::Draw(bool bAlphaParts)
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
			
			ID3D11InputLayout *pLayout = nullptr;
			HRCheck(part->inputLayout.CopyTo(&pLayout));
			D3DInputLayout vertexLayout;
			vertexLayout.MakeObject(pLayout);

			ID3D11Buffer *pVB = nullptr;
			HRCheck(part->vertexBuffer.CopyTo(&pVB));
			D3DVertexBuffer vertexBuffer;
			vertexBuffer.Buffer.MakeObject(pVB);
			vertexBuffer.Stride = part->vertexStride;
			vertexBuffer.Offset = 0U;

			ID3D11Buffer *pIB = nullptr;
			HRCheck(part->indexBuffer.CopyTo(&pIB));
			D3DIndexBuffer indexBuffer;
			indexBuffer.Buffer.MakeObject(pIB);
			indexBuffer.Format = part->indexFormat;
			indexBuffer.Offset = 0U;

			D3DEngine::Instance().SetInputLayout(vertexLayout);
			D3DEngine::Instance().SetVertexBuffer(vertexBuffer.Buffer, vertexBuffer.Stride, vertexBuffer.Offset);
			D3DEngine::Instance().SetIndexBuffer(indexBuffer.Buffer, indexBuffer.Format, indexBuffer.Offset);

			D3DEngine::Instance().DrawIndexed(part->indexCount, part->startIndex, part->vertexOffset, part->primitiveType);
		}
	}
}

NamespaceEnd(Geometry)