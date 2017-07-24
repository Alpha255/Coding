#include "D3DModel.h"
#include "D3DGraphic.h"
#include "Camera.h"

extern D3DGraphic* g_Renderer;

D3DModel::D3DModel()
	: m_EffectFactory(nullptr)
	, m_States(nullptr)
{
}

void D3DModel::CreateFromSDKMesh(const wchar_t* pName)
{
	assert(pName);

	if (!m_EffectFactory)
	{
		m_EffectFactory = new DirectX::EffectFactory(g_Renderer->m_D3DDevice.GetPtr());
	}

	if (!m_States)
	{
		m_States = new DirectX::CommonStates(g_Renderer->m_D3DDevice.GetPtr());
	}

	std::string meshFileDir = D3DGraphic::ResourceFileDirectory(D3DGraphic::eSDKMesh);

	///size_t size = meshFilePath.size() + 1U;
	///wchar_t* pMeshFilePath = new wchar_t[size]();
	///mbstowcs_s(nullptr, pMeshFilePath, size, meshFilePath.c_str(), size);

	char workingDir[MAX_PATH] = { 0 };
	::GetCurrentDirectoryA(MAX_PATH, workingDir);

	::SetCurrentDirectoryA(meshFileDir.c_str());
	m_Model = DirectX::Model::CreateFromSDKMESH(g_Renderer->m_D3DDevice.GetPtr(), pName, *m_EffectFactory, true, true);
	::SetCurrentDirectoryA(workingDir);
}

void D3DModel::Draw(const Camera& camera, bool bWireframe)
{
	g_Renderer->FlushPipelineState();

	m_Model->Draw(g_Renderer->m_D3DContext.GetPtr(), *m_States, camera.GetWorldMatrix(), camera.GetViewMatrix(), camera.GetProjMatrix(), bWireframe);

	g_Renderer->m_SwapChain->Present(0U, 0U);
}

const ID3D11Buffer* const D3DModel::GetVertexBuffer(uint32_t index) const
{
	assert(index >= 0 && index < m_Model->meshes.size());

	const ID3D11Buffer* const pVB = m_Model->meshes.at(index)->meshParts.at(index)->vertexBuffer.Get();

	return pVB;
}

const ID3D11Buffer* const D3DModel::GetIndexBuffer(uint32_t index) const
{
	assert(index >= 0 && index < m_Model->meshes.size());

	const ID3D11Buffer* const pIB = m_Model->meshes.at(index)->meshParts.at(index)->indexBuffer.Get();

	return pIB;
}

uint32_t D3DModel::GetVertexStride(uint32_t index) const
{
	assert(index >= 0 && index < m_Model->meshes.size());

	return m_Model->meshes.at(index)->meshParts.at(index)->vertexStride;
}

DXGI_FORMAT D3DModel::GetIndexFormat(uint32_t index) const
{
	assert(index >= 0 && index < m_Model->meshes.size());

	return m_Model->meshes.at(index)->meshParts.at(index)->indexFormat;
}

D3DModel::~D3DModel()
{
	if (m_EffectFactory)
	{
		SafeDelete(m_EffectFactory);
	}

	if (m_States)
	{
		SafeDelete(m_States);
	}
}