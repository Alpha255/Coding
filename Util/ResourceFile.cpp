#include "ResourceFile.h"
#include "System.h"

#include <fstream>

ResourceFile::ResourceFile(const char *pFileName)
{
	assert(pFileName);

	GetPath(pFileName);

	if (!IsValid())
	{
		return;
	}

	std::ifstream file(m_Path, eTexture == m_Type ? std::ios::in | std::ios::binary : std::ios::in);
	assert(file.good());

	file.seekg(0U, std::ios::end);
	m_Size = (size_t)file.tellg();
	file.close();
}

void ResourceFile::GetPath(const char *pFileName)
{
	assert(pFileName);

	m_Type = GetType(pFileName);
	m_Path = std::string(pFileName);

	if (!System::FileExists(pFileName))
	{
		static const char* s_ResourcePath[eTypeCount] =
		{
			"\\Resource\\Shaders\\",
			"\\Resource\\Textures\\",
			"\\Resource\\SDKMeshs\\",
			"\\Resource\\TxtMeshs\\",
			"\\Resource\\ObjMeshs\\",
			"\\Resource\\Sounds\\",
			"\\Resource\\LevelData\\",
		};
		char directory[MAX_PATH] = {};
		::GetModuleFileNameA(::GetModuleHandle(nullptr), directory, MAX_PATH);

		std::string appDir(directory);
		m_Path = appDir.substr(0, appDir.rfind("\\")) + s_ResourcePath[m_Type] + pFileName;

		if (!System::FileExists(m_Path.c_str()))
		{
			m_Type = eTypeCount;
		}
	}
}

ResourceFile::eType ResourceFile::GetType(const char *pFileName)
{
	if (System::IsStrEndwith(pFileName, ".hlsl") || System::IsStrEndwith(pFileName, ".glsl"))
	{
		return eShader;
	}
	else if (System::IsStrEndwith(pFileName, ".dds") || System::IsStrEndwith(pFileName, ".bmp"))
	{
		return eTexture;
	}
	else if (System::IsStrEndwith(pFileName, ".sdkmesh"))
	{
		return eSDKMesh;
	}
	else if (System::IsStrEndwith(pFileName, ".txt"))
	{
		return eTxtMesh;
	}
	else if (System::IsStrEndwith(pFileName, ".obj"))
	{
		return eObjMesh;
	}
	else if (System::IsStrEndwith(pFileName, ".mp3"))
	{
		return eSound;
	}
	else if (System::IsStrEndwith(pFileName, ".dat"))
	{
		return eLevelData;
	}
	else
	{
		assert(!"Unknown resource type!");
	}

	return eTypeCount;
}

uint8_t *ResourceFile::Load()
{
	assert(IsValid());

	if (0U == m_Size)
	{
		return nullptr;
	}

	std::ifstream file(m_Path, eTexture == m_Type ? std::ios::in | std::ios::binary : std::ios::in);
	assert(file.good());

	uint8_t *pData = new uint8_t[m_Size]();
	file.read((char *)pData, m_Size);
	file.close();

	return pData;
}