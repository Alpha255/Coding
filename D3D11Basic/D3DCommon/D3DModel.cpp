#include "D3DModel.h"
#include "D3DGraphic.h"
#include "Camera.h"

#include <fstream>
#include <sstream>

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
		m_EffectFactory = new DirectX::EffectFactory(g_Renderer->GetDevice());
	}

	if (!m_States)
	{
		m_States = new DirectX::CommonStates(g_Renderer->GetDevice());
	}

	std::string meshFileDir = D3DGraphic::ResourceFileDirectory(D3DGraphic::eSDKMesh);

	///size_t size = meshFilePath.size() + 1U;
	///wchar_t* pMeshFilePath = new wchar_t[size]();
	///mbstowcs_s(nullptr, pMeshFilePath, size, meshFilePath.c_str(), size);

	char workingDir[MAX_PATH] = { 0 };
	::GetCurrentDirectoryA(MAX_PATH, workingDir);

	::SetCurrentDirectoryA(meshFileDir.c_str());
	m_Model = DirectX::Model::CreateFromSDKMESH(g_Renderer->GetDevice(), pName, *m_EffectFactory, true, true);
	::SetCurrentDirectoryA(workingDir);
}

void D3DModel::Draw(const Camera& camera, bool bWireframe)
{
	///g_Renderer->FlushState();

	m_Model->Draw(g_Renderer->GetIMContext(), *m_States, camera.GetWorldMatrix(), camera.GetViewMatrix(), camera.GetProjMatrix(), bWireframe);

	///g_Renderer->m_SwapChain->Present(0U, 0U);
}

///void D3DModel::DrawCustom()
///{
///	g_Renderer->FlushState();
///
///	for (auto it_m = m_Model->meshes.cbegin(); it_m != m_Model->meshes.cend(); ++it_m)
///	{
///		for (auto it_mp = (*it_m)->meshParts.cbegin(); it_mp != (*it_m)->meshParts.cend(); ++it_mp)
///		{
///			auto part = (*it_mp).get();
///			assert(part);
///
///			ID3D11Buffer* pVertexBuffer = part->vertexBuffer.Get();
///			uint32_t vertexStride = part->vertexStride;
///			uint32_t vertexOffset = part->vertexOffset;
///			if (g_Renderer->m_D3DPipelineState.VertexBuffer.Stride[0] != vertexStride ||
///				g_Renderer->m_D3DPipelineState.VertexBuffer.Offset[0] != vertexOffset ||
///				g_Renderer->m_D3DPipelineState.VertexBuffer.Buffers[0] != pVertexBuffer)
///			{
///				g_Renderer->m_D3DContext->IASetVertexBuffers(0U, 1U, &pVertexBuffer, &vertexStride, &vertexOffset);
///			}
///
///			ID3D11Buffer* pIndexBuffer = part->indexBuffer.Get();
///			DXGI_FORMAT indexFormat = part->indexFormat;
///			if (g_Renderer->m_D3DPipelineState.IndexBuffer.Format != indexFormat ||
///				g_Renderer->m_D3DPipelineState.IndexBuffer.Buffers != pIndexBuffer)
///			{
///				g_Renderer->m_D3DContext->IASetIndexBuffer(pIndexBuffer, indexFormat, 0U);
///			}
///
///			if (g_Renderer->m_D3DPipelineState.PrimitiveTopology != part->primitiveType)
///			{
///				g_Renderer->m_D3DContext->IASetPrimitiveTopology(part->primitiveType);
///			}
///
///			g_Renderer->m_D3DContext->DrawIndexed(part->indexCount, part->startIndex, part->vertexOffset);
///		}
///	}
///
///	g_Renderer->m_SwapChain->Present(0U, 0U);
///}

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

void SDKMesh::CreateFromTxt(const char *pName)
{
	assert(pName && g_Renderer);

	std::string meshFileDir = D3DGraphic::ResourceFileDirectory(D3DGraphic::eTxtMesh);
	meshFileDir += pName;

	std::ifstream meshFile(meshFileDir.c_str(), std::ios::in);
	if (meshFile.good())
	{
		uint32_t triangleCount = 0U;
		std::string ignore;

		meshFile >> ignore >> m_VertexCount;
		meshFile >> ignore >> triangleCount;
		meshFile >> ignore >> ignore >> ignore >> ignore;

		std::vector<Vertex> vertices(m_VertexCount);
		for (uint32_t i = 0U; i < m_VertexCount; ++i)
		{
			meshFile >> vertices[i].Position.x >> vertices[i].Position.y >> vertices[i].Position.z;
			meshFile >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
		}

		meshFile >> ignore;
		meshFile >> ignore;
		meshFile >> ignore;

		m_IndexCount = 3 * triangleCount;
		std::vector<uint32_t> indices(m_IndexCount);
		for (uint32_t i = 0U; i < triangleCount; ++i)
		{
			meshFile >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
		}

		meshFile.close();

		static char* const s_ShaderName = "Lighting.hlsl";
		
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		g_Renderer->CreateVertexShaderAndInputLayout(m_D3DRes.VertexShader.Reference(), m_D3DRes.InputLayout.Reference(), layout, ARRAYSIZE(layout), 
			s_ShaderName, "VSMain");
		g_Renderer->CreatePixelShader(m_D3DRes.PixelShader.Reference(), s_ShaderName, "PSMain");

		g_Renderer->CreateVertexBuffer(m_D3DRes.VertexBuffer.Reference(), sizeof(Vertex) * m_VertexCount, D3D11_USAGE_IMMUTABLE, &vertices[0]);
		g_Renderer->CreateIndexBuffer(m_D3DRes.IndexBuffer.Reference(), sizeof(uint32_t) * m_IndexCount, D3D11_USAGE_IMMUTABLE, &indices[0]);

		g_Renderer->CreateConstantBuffer(m_D3DRes.CBufferVS.Reference(), sizeof(ConstantsBufferVS),
			D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
		g_Renderer->CreateConstantBuffer(m_D3DRes.CBufferPS.Reference(), sizeof(ConstantsBufferPS),
			D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

		g_Renderer->CreateRasterizerState(m_D3DRes.WireframeMode.Reference(), D3D11_FILL_WIREFRAME);

		g_Renderer->CreateRasterizerState(m_D3DRes.BackFaceCulling.Reference(), D3D11_FILL_SOLID, D3D11_CULL_BACK);

		m_Created = true;
	}
	else
	{
		assert(0);
	}
}

void SDKMesh::Draw(const Camera &cam, bool bWireframe)
{
	if (!m_Created || !g_Renderer)
	{
		return;
	}

	g_Renderer->SetRasterizerState(m_D3DRes.BackFaceCulling.Ptr());

	g_Renderer->SetVertexShader(m_D3DRes.VertexShader.Ptr());
	g_Renderer->SetPixelShader(m_D3DRes.PixelShader.Ptr());

	g_Renderer->SetInputLayout(m_D3DRes.InputLayout.Ptr());
	g_Renderer->SetVertexBuffer(m_D3DRes.VertexBuffer.Ptr(), sizeof(Vertex), 0U);
	g_Renderer->SetIndexBuffer(m_D3DRes.IndexBuffer.Ptr(), DXGI_FORMAT_R32_UINT);

	m_CBufferVS.World = m_World.Transpose();
	m_CBufferVS.WorldInverseTrans = m_CBufferVS.World.Inverse();
	Matrix wvp = m_World * cam.GetViewMatrix() * cam.GetProjMatrix();
	m_CBufferVS.WVP = wvp.Transpose();

	Vec4 eyePos = cam.GetEyePos();
	m_CBufferPS.ViewPoint = Vec3(eyePos.x, eyePos.y, eyePos.z);

	g_Renderer->UpdateBuffer(m_D3DRes.CBufferVS.Ptr(), &m_CBufferVS, sizeof(ConstantsBufferVS));
	g_Renderer->UpdateBuffer(m_D3DRes.CBufferPS.Ptr(), &m_CBufferPS, sizeof(ConstantsBufferPS));

	g_Renderer->SetConstantBuffer(m_D3DRes.CBufferVS.Ptr(), 0U, D3DGraphic::eVertexShader);
	g_Renderer->SetConstantBuffer(m_D3DRes.CBufferPS.Ptr(), 0U, D3DGraphic::ePixelShader);

	if (bWireframe)
	{
		g_Renderer->SetRasterizerState(m_D3DRes.WireframeMode.Ptr());
	}

	g_Renderer->DrawIndexed(m_IndexCount, 0U, 0);

	m_CBufferVS.World.Identity();
}

void ObjMesh::Create(const char *pFileName)
{
	assert(pFileName && g_Renderer);

	std::string meshFileDir = D3DGraphic::ResourceFileDirectory(D3DGraphic::eObjMesh);
	meshFileDir += pFileName;

	std::ifstream meshFile(meshFileDir.c_str(), std::ios::in);
	if (meshFile.good())
	{
		char line[MAX_PATH] = { 0 };
		std::vector<Vec3> vertices;
		std::vector<uint32_t> indices;

		while (meshFile >> line)
		{
			if (0 == strcmp(line, "#"))
			{
				meshFile.getline(line, MAX_PATH);
			}
			else if (0 == strcmp(line, "v"))   		/// Read vertices
			{
				Vec3 v;
				meshFile >> v.x >> v.y >> v.z;
				vertices.push_back(v);
			}
			else if (0 == strcmp(line, "f"))  		/// Read indices
			{
				meshFile.getline(line, MAX_PATH);
				
				std::stringstream strStream(line);
				uint32_t index[4] = { 0U };
				uint32_t i = 0U;
				while (strStream >> line)
				{
					std::stringstream ss(line);
					ss >> index[i++];
					///int32_t idx = atoi(line);
					///assert(idx >= 0);
					///index[i++] = (uint32_t)idx;
				}

				indices.push_back(index[0] - 1);
				indices.push_back(index[1] - 1);
				indices.push_back(index[2] - 1);

				/// Quad face?
				if (i >= 4U)
				{
					indices.push_back(index[2] - 1);
					indices.push_back(index[3] - 1);
					indices.push_back(index[0] - 1);
				}
			}
		}

		meshFile.close();

		CreateVIBuffer(vertices, indices);
	}
	else
	{
		assert(0);
	}
}

void ObjMesh::CreateVIBuffer(const std::vector<Vec3> &vertices, const std::vector<uint32_t> &indices)
{
	m_VertexCount = (uint32_t)vertices.size();
	m_IndexCount = (uint32_t)indices.size();

	assert(m_VertexCount && m_IndexCount);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	Ref<ID3D11VertexShader> vs;
	static char *const s_ShaderName = "ObjFake.hlsl";

	g_Renderer->CreateVertexShaderAndInputLayout(vs.Reference(), m_InputLayout.Reference(), layout, ARRAYSIZE(layout), s_ShaderName, "VS_Main");

	g_Renderer->CreateVertexBuffer(m_VertexBuffer.Reference(), sizeof(Vec3) * m_VertexCount, D3D11_USAGE_IMMUTABLE, &vertices[0]);

	g_Renderer->CreateIndexBuffer(m_IndexBuffer.Reference(), sizeof(uint32_t) * m_IndexCount, D3D11_USAGE_IMMUTABLE, &indices[0]);
}