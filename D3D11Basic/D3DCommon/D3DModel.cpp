#include "D3DModel.h"
//#include "D3DGraphic.h"
//#include "Camera.h"
//#include "Utility.h"
//
//#include <fstream>
//#include <sstream>
//
//extern D3DGraphic* g_Renderer;
//
//D3DModel::D3DModel()
//	: m_EffectFactory(nullptr)
//	, m_States(nullptr)
//{
//}
//
//void D3DModel::CreateFromSDKMesh(const wchar_t* pName)
//{
//	assert(pName);
//
//	if (!m_EffectFactory)
//	{
//		m_EffectFactory = new DirectX::EffectFactory(g_Renderer->GetDevice());
//	}
//
//	if (!m_States)
//	{
//		m_States = new DirectX::CommonStates(g_Renderer->GetDevice());
//	}
//
//	std::string meshFileDir = Utility::ResourceFileDirectory(Utility::eSDKMesh);
//
//	///size_t size = meshFilePath.size() + 1U;
//	///wchar_t* pMeshFilePath = new wchar_t[size]();
//	///mbstowcs_s(nullptr, pMeshFilePath, size, meshFilePath.c_str(), size);
//
//	char workingDir[MAX_PATH] = { 0 };
//	::GetCurrentDirectoryA(MAX_PATH, workingDir);
//
//	::SetCurrentDirectoryA(meshFileDir.c_str());
//	m_Model = DirectX::Model::CreateFromSDKMESH(g_Renderer->GetDevice(), pName, *m_EffectFactory, true, true);
//	::SetCurrentDirectoryA(workingDir);
//}
//
//void D3DModel::Draw(const Camera& camera, bool bWireframe)
//{
//	///g_Renderer->FlushState();
//
//	m_Model->Draw(g_Renderer->GetIMContext(), *m_States, camera.GetWorldMatrix(), camera.GetViewMatrix(), camera.GetProjMatrix(), bWireframe);
//
//	///g_Renderer->m_SwapChain->Present(0U, 0U);
//}
//
/////void D3DModel::DrawCustom()
/////{
/////	g_Renderer->FlushState();
/////
/////	for (auto it_m = m_Model->meshes.cbegin(); it_m != m_Model->meshes.cend(); ++it_m)
/////	{
/////		for (auto it_mp = (*it_m)->meshParts.cbegin(); it_mp != (*it_m)->meshParts.cend(); ++it_mp)
/////		{
/////			auto part = (*it_mp).get();
/////			assert(part);
/////
/////			ID3D11Buffer* pVertexBuffer = part->vertexBuffer.Get();
/////			uint32_t vertexStride = part->vertexStride;
/////			uint32_t vertexOffset = part->vertexOffset;
/////			if (g_Renderer->m_PipelineState.VertexBuffer.Stride[0] != vertexStride ||
/////				g_Renderer->m_PipelineState.VertexBuffer.Offset[0] != vertexOffset ||
/////				g_Renderer->m_PipelineState.VertexBuffer.Buffers[0] != pVertexBuffer)
/////			{
/////				g_Renderer->m_IMContent->IASetVertexBuffers(0U, 1U, &pVertexBuffer, &vertexStride, &vertexOffset);
/////			}
/////
/////			ID3D11Buffer* pIndexBuffer = part->indexBuffer.Get();
/////			DXGI_FORMAT indexFormat = part->indexFormat;
/////			if (g_Renderer->m_PipelineState.IndexBuffer.Format != indexFormat ||
/////				g_Renderer->m_PipelineState.IndexBuffer.Buffers != pIndexBuffer)
/////			{
/////				g_Renderer->m_IMContent->IASetIndexBuffer(pIndexBuffer, indexFormat, 0U);
/////			}
/////
/////			if (g_Renderer->m_PipelineState.PrimitiveTopology != part->primitiveType)
/////			{
/////				g_Renderer->m_IMContent->IASetPrimitiveTopology(part->primitiveType);
/////			}
/////
/////			g_Renderer->m_IMContent->DrawIndexed(part->indexCount, part->startIndex, part->vertexOffset);
/////		}
/////	}
/////
/////	g_Renderer->m_SwapChain->Present(0U, 0U);
/////}
//
//const ID3D11Buffer* const D3DModel::GetVertexBuffer(uint32_t index) const
//{
//	assert(index >= 0 && index < m_Model->meshes.size());
//
//	const ID3D11Buffer* const pVB = m_Model->meshes.at(index)->meshParts.at(index)->vertexBuffer.Get();
//
//	return pVB;
//}
//
//const ID3D11Buffer* const D3DModel::GetIndexBuffer(uint32_t index) const
//{
//	assert(index >= 0 && index < m_Model->meshes.size());
//
//	const ID3D11Buffer* const pIB = m_Model->meshes.at(index)->meshParts.at(index)->indexBuffer.Get();
//
//	return pIB;
//}
//
//uint32_t D3DModel::GetVertexStride(uint32_t index) const
//{
//	assert(index >= 0 && index < m_Model->meshes.size());
//
//	return m_Model->meshes.at(index)->meshParts.at(index)->vertexStride;
//}
//
//DXGI_FORMAT D3DModel::GetIndexFormat(uint32_t index) const
//{
//	assert(index >= 0 && index < m_Model->meshes.size());
//
//	return m_Model->meshes.at(index)->meshParts.at(index)->indexFormat;
//}
//
//D3DModel::~D3DModel()
//{
//	if (m_EffectFactory)
//	{
//		SafeDelete(m_EffectFactory);
//	}
//
//	if (m_States)
//	{
//		SafeDelete(m_States);
//	}
//}
//
//void SDKMesh::CreateFromTxt(const char *pName)
//{
//	assert(pName && g_Renderer);
//
//	std::string meshFilePath = Utility::ResourceFilePath(pName, Utility::eTxtMesh);
//
//	std::ifstream meshFile(meshFilePath.c_str(), std::ios::in);
//	if (meshFile.good())
//	{
//		uint32_t triangleCount = 0U;
//		std::string ignore;
//
//		meshFile >> ignore >> m_VertexCount;
//		meshFile >> ignore >> triangleCount;
//		meshFile >> ignore >> ignore >> ignore >> ignore;
//
//		std::vector<Vertex> vertices(m_VertexCount);
//		for (uint32_t i = 0U; i < m_VertexCount; ++i)
//		{
//			meshFile >> vertices[i].Position.x >> vertices[i].Position.y >> vertices[i].Position.z;
//			meshFile >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
//		}
//
//		meshFile >> ignore;
//		meshFile >> ignore;
//		meshFile >> ignore;
//
//		m_IndexCount = 3 * triangleCount;
//		std::vector<uint32_t> indices(m_IndexCount);
//		for (uint32_t i = 0U; i < triangleCount; ++i)
//		{
//			meshFile >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
//		}
//
//		meshFile.close();
//
//		static char* const s_ShaderName = "Lighting.hlsl";
//		
//		D3D11_INPUT_ELEMENT_DESC layout[] =
//		{
//			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
//			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//		};
//		g_Renderer->CreateVertexShaderAndInputLayout(m_D3DRes.VertexShader, m_D3DRes.InputLayout, layout, ARRAYSIZE(layout), 
//			s_ShaderName, "VSMain");
//		g_Renderer->CreatePixelShader(m_D3DRes.PixelShader, s_ShaderName, "PSMain");
//
//		g_Renderer->CreateVertexBuffer(m_D3DRes.VertexBuffer, sizeof(Vertex) * m_VertexCount, D3D11_USAGE_IMMUTABLE, &vertices[0]);
//		g_Renderer->CreateIndexBuffer(m_D3DRes.IndexBuffer, sizeof(uint32_t) * m_IndexCount, D3D11_USAGE_IMMUTABLE, &indices[0]);
//
//		g_Renderer->CreateConstantBuffer(m_D3DRes.CBufferVS, sizeof(ConstantsBufferVS),
//			D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
//		g_Renderer->CreateConstantBuffer(m_D3DRes.CBufferPS, sizeof(ConstantsBufferPS),
//			D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
//
//		g_Renderer->CreateRasterizerState(m_D3DRes.WireframeMode, D3D11_FILL_WIREFRAME);
//
//		g_Renderer->CreateRasterizerState(m_D3DRes.BackFaceCulling, D3D11_FILL_SOLID, D3D11_CULL_BACK);
//
//		m_Created = true;
//	}
//	else
//	{
//		assert(0);
//	}
//}
//
//void SDKMesh::Draw(const Camera &cam, bool bWireframe)
//{
//	if (!m_Created || !g_Renderer)
//	{
//		return;
//	}
//
//	///g_Renderer->SetRasterizerState(m_D3DRes.BackFaceCulling);
//
//	g_Renderer->SetVertexShader(m_D3DRes.VertexShader);
//	g_Renderer->SetPixelShader(m_D3DRes.PixelShader);
//
//	g_Renderer->SetInputLayout(m_D3DRes.InputLayout);
//	g_Renderer->SetVertexBuffer(m_D3DRes.VertexBuffer, sizeof(Vertex), 0U);
//	g_Renderer->SetIndexBuffer(m_D3DRes.IndexBuffer, DXGI_FORMAT_R32_UINT);
//
//	m_CBufferVS.World = m_World.Transpose();
//	m_CBufferVS.WorldInverseTrans = m_CBufferVS.World.Inverse();
//	Matrix wvp = m_World * cam.GetViewMatrix() * cam.GetProjMatrix();
//	m_CBufferVS.WVP = wvp.Transpose();
//
//	Vec4 eyePos = cam.GetEyePos();
//	m_CBufferPS.ViewPoint = Vec3(eyePos.x, eyePos.y, eyePos.z);
//
//	g_Renderer->UpdateBuffer(m_D3DRes.CBufferVS, &m_CBufferVS, sizeof(ConstantsBufferVS));
//	g_Renderer->UpdateBuffer(m_D3DRes.CBufferPS, &m_CBufferPS, sizeof(ConstantsBufferPS));
//
//	g_Renderer->SetConstantBuffer(m_D3DRes.CBufferVS, 0U, D3DGraphic::eVertexShader);
//	g_Renderer->SetConstantBuffer(m_D3DRes.CBufferPS, 0U, D3DGraphic::ePixelShader);
//
//	if (bWireframe)
//	{
//		g_Renderer->SetRasterizerState(m_D3DRes.WireframeMode);
//	}
//
//	g_Renderer->DrawIndexed(m_IndexCount, 0U, 0);
//
//	///Vec4 pos = m_CBufferPS.DirLight[m_CBufferPS.LightCount - 1].Direction;
//	///Lighting::DrawLight(Vec3(pos.x, pos.y, pos.z), cam);
//
//	m_CBufferVS.World.Identity();
//}
//
//void ObjMesh::Create(const char *pFileName)
//{
//	assert(pFileName && g_Renderer);
//
//	std::string meshFilePath = Utility::ResourceFilePath(pFileName, Utility::eObjMesh);
//
//	std::ifstream meshFile(meshFilePath.c_str(), std::ios::in);
//	if (meshFile.good())
//	{
//		char line[MAX_PATH] = { 0 };
//		std::vector<Vec3> vertices;
//		std::vector<ObjIndex> indices;
//		std::vector<Vec3> normals;
//		std::vector<Vec2> uvs;
//
//		while (meshFile >> line)
//		{
//			if (0 == strcmp(line, "#"))
//			{
//				meshFile.getline(line, MAX_PATH);
//			}
//			else if (0 == strcmp(line, "v"))   		/// Read vertices
//			{
//				Vec3 v;
//				meshFile >> v.x >> v.y >> v.z;
//				vertices.push_back(v);
//			}
//			else if (0 == strcmp(line, "vn"))       /// Read normals
//			{
//				Vec3 vn;
//				meshFile >> vn.x >> vn.y >> vn.z;
//				normals.push_back(vn);
//			}
//			else if (0 == strcmp(line, "vt"))   	/// Read uvs
//			{
//				Vec2 vt;
//				meshFile >> vt.x >> vt.y;
//				uvs.push_back(vt);
//			}
//			else if (0 == strcmp(line, "f"))  		/// Read indices
//			{
//				meshFile.getline(line, MAX_PATH);
//
//				/// f 1      i
//				/// f 1/1    i/t
//				/// f 1//1   i/n
//				/// f 1/1/1  i/t/n
//				ObjIndex index[4];
//				const char *pStrBeg = &line[1];
//				
//				std::stringstream ss(pStrBeg);
//				uint32_t m = 0U;
//				while (!ss.eof())
//				{
//					ss >> index[m].i;
//					const char *pStr = pStrBeg + ss.tellg();
//
//					if (' ' == *pStr)
//					{
//						++m;
//						continue;
//					}
//					else if ('/' == *pStr)
//					{
//						ss.get();
//						pStr = pStrBeg + ss.tellg();
//						if ('/' == *pStr)
//						{
//							ss.get();
//							ss >> index[m].n;
//						}
//						else
//						{
//							ss >> index[m].t;
//							pStr = pStrBeg + ss.tellg();
//							if (' ' == *pStr)
//							{
//								++m;
//								continue;
//							}
//							else if ('/' == *pStr)
//							{
//								ss.get();
//								ss >> index[m].n;
//							}
//							else
//							{
//								assert(!"Invalid obj file!!");
//							}
//						}
//					}
//					else
//					{
//						assert(!"Invalid obj file!!");
//					}
//
//					++m;
//				}
//
//				indices.insert(indices.end(), &index[0], &index[0] + 4);
//			}
//		}
//
//		meshFile.close();
//
//		CreateVI(vertices, indices, normals, uvs);
//	}
//	else
//	{
//		assert(0);
//	}
//}
//
//void ObjMesh::CreateVI(
//	const std::vector<Vec3> &srcVertices, 
//	const std::vector<ObjIndex> &objIndices, 
//	const std::vector<Vec3> &normals, 
//	const std::vector<Vec2> &uvs)
//{
//	for (size_t i = 0U; i < objIndices.size(); i += 4)
//	{
//		for (size_t j = i; j < i + 3U; ++j)
//		{
//			const ObjIndex &face = objIndices.at(j);
//
//			Math::Geometry::BasicVertex v;
//			v.Position = srcVertices.at(face.i - 1U);
//			v.Normal = face.n > 0U ? normals.at(face.n - 1U) : Vec3(0.0f, 0.0f, 0.0f);
//			v.UV = face.t > 0U ? uvs.at(face.t - 1U) : Vec2(0.0f, 0.0f);
//
//			m_Vertices.push_back(v);
//			m_Indices.push_back(face.i - 1U);
//		}
//
//		if (objIndices.at(i + 3U).i > 0U)  /// Quad ?
//		{
//			Math::Geometry::BasicVertex v;
//
//			size_t i0 = i + 2U;
//			const ObjIndex &face0 = objIndices.at(i0);
//			v.Position = srcVertices.at(face0.i - 1U);
//			v.Normal = face0.n > 0U ? normals.at(face0.n - 1U) : Vec3(0.0f, 0.0f, 0.0f);
//			v.UV = face0.t > 0U ? uvs.at(face0.t - 1U) : Vec2(0.0f, 0.0f);
//			m_Vertices.push_back(v);
//			m_Indices.push_back(face0.i - 1U);
//
//			size_t i1 = i + 3U;
//			const ObjIndex &face1 = objIndices.at(i1);
//			v.Position = srcVertices.at(face1.i - 1U);
//			v.Normal = face1.n > 0U ? normals.at(face1.n - 1U) : Vec3(0.0f, 0.0f, 0.0f);
//			v.UV = face1.t > 0U ? uvs.at(face1.t - 1U) : Vec2(0.0f, 0.0f);
//			m_Vertices.push_back(v);
//			m_Indices.push_back(face1.i - 1U);
//
//			size_t i2 = i;
//			const ObjIndex &face2 = objIndices.at(i2);
//			v.Position = srcVertices.at(face2.i - 1U);
//			v.Normal = face2.n > 0U ? normals.at(face2.n - 1U) : Vec3(0.0f, 0.0f, 0.0f);
//			v.UV = face2.t > 0U ? uvs.at(face2.t - 1U) : Vec2(0.0f, 0.0f);
//
//			m_Vertices.push_back(v);
//			m_Indices.push_back(face2.i - 1U);
//		}
//	}
//}