#include "D3DGeometry.h"
#include "D3DEngine.h"
#include "Camera.h"
#include "System.h"

#include <fstream>
#include <sstream>

NamespaceBegin(Geometry)

void ObjMesh::Create(const char *pFileName)
{
	assert(pFileName);

	std::string meshFilePath = System::ResourceFilePath(pFileName, System::eObjMesh);

	std::ifstream meshFile(meshFilePath.c_str(), std::ios::in);
	if (meshFile.good())
	{
		char line[MAX_PATH] = { 0 };
		std::vector<Vec3> vertices;
		std::vector<ObjIndex> indices;
		std::vector<std::vector<ObjIndex>> indexColloctor;
		std::vector<Vec3> normals;
		std::vector<Vec2> uvs;

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
			else if (0 == strcmp(line, "vn"))       /// Read normals
			{
				Vec3 vn;
				meshFile >> vn.x >> vn.y >> vn.z;
				normals.push_back(vn);
			}
			else if (0 == strcmp(line, "vt"))   	/// Read uvs
			{
				Vec2 vt;
				meshFile >> vt.x >> vt.y;
				uvs.push_back(vt);
			}
			else if (0 == strcmp(line, "f"))  		/// Read faces
			{
				meshFile.getline(line, MAX_PATH);

				/// f 1      i
				/// f 1/1    i/t
				/// f 1//1   i/n
				/// f 1/1/1  i/t/n
				ObjIndex index;
				const char *pStrBeg = &line[1];

				std::stringstream ss(pStrBeg);
				indices.clear();
				while (!ss.eof())
				{
					ss >> index.i;
					const char *pStr = pStrBeg + ss.tellg();

					if (' ' == *pStr)
					{
						indices.push_back(index);
						continue;
					}
					else if ('/' == *pStr)
					{
						ss.get();
						pStr = pStrBeg + ss.tellg();
						if ('/' == *pStr)
						{
							ss.get();
							ss >> index.n;
							indices.push_back(index);
						}
						else
						{
							ss >> index.t;
							pStr = pStrBeg + ss.tellg();
							if (' ' == *pStr)
							{
								indices.push_back(index);
								continue;
							}
							else if ('/' == *pStr)
							{
								ss.get();
								ss >> index.n;
								indices.push_back(index);
							}
							else
							{
								assert(!"Invalid obj file!!");
							}
						}
					}
					else
					{
						assert(!"Invalid obj file!!");
					}
				}

				indexColloctor.push_back(indices);
			}
		}

		meshFile.close();

		CreateVertexData(vertices, indexColloctor, normals, uvs);

		ComputeTangent();

		CreateRenderResource();
	}
	else
	{
		assert(0);
	}
}

void ObjMesh::CreateVertexData(
	const std::vector<Vec3> &srcVertices,
	const std::vector<std::vector<ObjIndex>> &indexCollector,
	const std::vector<Vec3> &normals,
	const std::vector<Vec2> &uvs)
{
	/*
	*  A face polygon composed of more than three vertices is triangulated
	*  according to the following schema:
	*                      v5
	*                     /  \
	*                    /    \
	*                   /      \
	*                  v1------v4
	*                  |\      /
	*                  | \    /
	*                  |  \  /
	*                 v2---v3
	*
	*  As shown above, the 5 vertices polygon (v1,v2,v3,v4,v5)
	*  has been split into the triangles (v1,v2,v3), (v1,v3,v4) e (v1,v4,v5).
	*  This way vertex v1 becomes the common vertex of all newly generated
	*  triangles, and this may lead to the creation of very thin triangles.
	*/
	for (uint32_t i = 0U; i < indexCollector.size(); ++i)
	{
		assert(indexCollector[i].size() >= 3U);

		for (uint32_t j = 0U; j < indexCollector[i].size(); ++j)
		{
			const ObjIndex &face = indexCollector[i][j];
			assert(face.i >= 0 && face.n >= 0U && face.t >= 0U);

			if (j < 3U)
			{
				/// v0, v1, v2
				m_Indices.push_back((uint32_t)m_Vertices.size());
			}
			else
			{
				uint32_t vertexCount = (uint32_t)m_Vertices.size();

				m_Indices.push_back(vertexCount - j);
				m_Indices.push_back(vertexCount - 1U);
				m_Indices.push_back(vertexCount);
			}

			Vertex v;
			v.Position = srcVertices.at(face.i - 1U);
			v.Normal = face.n > 0U ? normals.at(face.n - 1U) : Vec3(0.0f, 0.0f, 0.0f);
			v.UV = face.t > 0 ? uvs.at(face.t - 1U) : Vec2(0.0f, 0.0f);
			m_Vertices.push_back(v);
		}
	}
}

void ObjMesh::ComputeTangent()
{
	///for (uint32_t i = 0U; i < m_Indices.size() / 3U; i += 3U)
	///{
	///	Vec3 p1 = m_Vertices[m_Indices[i + 0U]].Position;
	///	Vec3 p2 = m_Vertices[m_Indices[i + 1U]].Position;
	///	Vec3 p3 = m_Vertices[m_Indices[i + 2U]].Position;
	
	///	Vec2 uv1 = m_Vertices[m_Indices[i + 0U]].UV;
	///	Vec2 uv2 = m_Vertices[m_Indices[i + 1U]].UV;
	///	Vec2 uv3 = m_Vertices[m_Indices[i + 2U]].UV;
	
	///	Vec3 edge1 = p2 - p1;
	///	Vec3 edge2 = p3 - p1;
	
	///	Vec2 deltaUV1 = uv2 - uv1;
	///	Vec2 deltaUV2 = uv3 - uv1;
	
	///	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	///	Vec3 tangent;
	///	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	///	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	///	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	
	///	m_Vertices[m_Indices[i + 0U]].Tangent = tangent;
	///	m_Vertices[m_Indices[i + 1U]].Tangent = tangent;
	///	m_Vertices[m_Indices[i + 2U]].Tangent = tangent;
	///}
}

NamespaceEnd(Geometry)