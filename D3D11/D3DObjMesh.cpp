#include "D3DGeometry.h"
#include "D3DEngine.h"
#include "Camera.h"
#include "System.h"

#include <sstream>

NamespaceBegin(Geometry)

void ObjMesh::TokenizeNextLine(std::ifstream &fs, std::vector<std::string> &tokens)
{
	if (fs.eof())
	{
		return;
	}

	std::string line;
	do
	{
		std::getline(fs, line);
		/// We have to manage backspace terminated lines, joining them together before parsing them
		if (!line.empty() && line.back() == 13)
		{
			line.pop_back();
		}

		while (!line.empty() && line.back() == '\\')
		{
			std::string tmp;
			std::getline(fs, tmp);
			if (tmp.back() == 13)
			{
				line.pop_back();
			}

			line.pop_back();
			line.append(tmp);
		}

		/// Handle vertex color here
	} while ((line.length() == 0 || line[0] == '#') && !fs.eof()); /// Skip comments and empty lines

	if (line.length() == 0 || line[0] == '#')
	{
		return;
	}

	size_t from = 0;
	size_t to = 0;
	size_t length = line.size();

	tokens.clear();
	do
	{
		while (from != length && (line[from] == ' ' || line[from] == '\t' || line[from] == '\r'))
		{
			++from;
		}

		if (from != length)
		{
			to = from + 1;
			while (to != length && line[to] != ' ' && line[to] != '\t' && line[to] != '\r')
			{
				++to;
			}

			tokens.push_back(line.substr(from, to - from).c_str());
			from = to;
		}
	} while (from < length);
}

void ObjMesh::Create(const char *pFileName)
{
	assert(pFileName && System::IsStrEndwith(pFileName, ".obj"));

	std::string filePath = System::ResourceFilePath(pFileName, System::eObjMesh);
	std::ifstream fileStream(filePath.c_str(), std::ios::in);

	assert(fileStream.good());

	std::vector<std::string> tokens;
	std::string header;
	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;
	std::vector<Vec2> uvs;

	while (!fileStream.eof())
	{
		TokenizeNextLine(fileStream, tokens);

		size_t numTokens = tokens.size();
		if (0U == numTokens)
		{
			continue;
		}

		header.clear();
		header = tokens[0];

		if ("v" == header) 		  /// Vertex
		{
			assert(numTokens >= 4U);
			vertices.push_back(Vec3((float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[3].c_str())));
			/// Handle vertex color here
		}
		else if ("vt" == header)  /// UV
		{
			assert(numTokens >= 3U);
			uvs.push_back(Vec2((float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str())));
		}
		else if ("vn" == header)  /// Normal
		{
			assert(numTokens == 4U);
			normals.push_back(Vec3((float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[3].c_str())));
		}
		else if ("l" == header)  /// Edge
		{
			assert(numTokens >= 3U);
		}
		else if ("f" == header || "q" == header)  /// Face
		{
		}
		else if ("mtllib" == header && numTokens > 1U)  /// Material library
		{
		}
		else if ("usemtl" == header && numTokens > 1U)  /// Material usage
		{
		}
	}

	fileStream.close();
}

NamespaceEnd(Geometry)