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

void SDKMesh::Draw(const Camera &cam, const Transform &transform, bool bWireframe)
{
	Matrix world = cam.GetWorldMatrix();
	world *= transform.Get();

	Matrix view = cam.GetViewMatrix();
	Matrix proj = cam.GetProjMatrix();

	///m_Model->Draw(D3DEngine::Instance().GetIMContext().Get(), *m_States, world, view, proj, bWireframe);

	/// Draw opaque parts
	for (auto it = m_Model->meshes.cbegin(); it != m_Model->meshes.cend(); ++it)
	{
		auto mesh = it->get();
		assert(mesh != 0);

		for (auto it = mesh->meshParts.cbegin(); it != mesh->meshParts.cend(); ++it)
		{
			auto part = (*it).get();
			assert(part != 0);

			if (part->isAlpha)
			{
				/// Skip alpha parts when drawing opaque or skip opaque parts if drawing alpha
				continue;
			}
			
			D3DInputLayout *vertexLayout = new D3DInputLayout;
			vertexLayout->MakeObject(part->inputLayout.Get());

			D3DVertexBuffer *vertexBuffer = new D3DVertexBuffer;
			vertexBuffer->Buffer.MakeObject(part->vertexBuffer.Get());
			vertexBuffer->Stride = part->vertexStride;
			vertexBuffer->Offset = 0U;

			D3DIndexBuffer *indexBuffer = new D3DIndexBuffer;
			indexBuffer->Buffer.MakeObject(part->indexBuffer.Get());
			indexBuffer->Format = part->indexFormat;
			indexBuffer->Offset = 0U;

			D3DEngine::Instance().SetInputLayout(*vertexLayout);
			D3DEngine::Instance().SetVertexBuffer(vertexBuffer->Buffer, vertexBuffer->Stride, vertexBuffer->Offset);
			D3DEngine::Instance().SetIndexBuffer(indexBuffer->Buffer, indexBuffer->Format, indexBuffer->Offset);

			D3DEngine::Instance().DrawIndexed(part->indexCount, part->startIndex, part->vertexOffset, part->primitiveType);
		}
	}

	/// Draw alpha parts
	///for (auto it = meshes.cbegin(); it != meshes.cend(); ++it)
	///{
	///	auto mesh = it->get();
	///	assert(mesh != 0);
	
	///	mesh->PrepareForRendering(deviceContext, states, true, wireframe);
	
	///	mesh->Draw(deviceContext, world, view, projection, true, setCustomState);
	///}
}

NamespaceEnd(Geometry)