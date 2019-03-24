#include "AssetFile.h"

AssetFile::AssetFile(const std::string &fileName)
	: m_Path(fileName)
{
	if (TryToFindAssetFile())
	{
		std::ifstream file(m_Path, eTexture == m_Type ? std::ios::in | std::ios::binary : std::ios::in);
		assert(file.good());

		file.seekg(0U, std::ios::end);
		m_Size = (size_t)file.tellg();
		file.close();
	}
}

bool AssetFile::TryToFindAssetFile()
{
	GetAssetType();

	static std::string s_AssetsPath[eTypeCount] =
	{
		"Assets\\Shaders\\",
		"Assets\\Shaders\\",
		"Assets\\Textures\\",
		"Assets\\SDKMeshs\\",
		"Assets\\TxtMeshs\\",
		"Assets\\ObjMeshs\\",
		"Assets\\Sounds\\",
		"Assets\\LevelData\\",
	};
	char curPath[MAX_PATH] = {};
	Verify(::GetCurrentDirectoryA(MAX_PATH, curPath) != 0);

	std::string assetPath(curPath);
	assetPath += "\\..\\";
	assetPath += s_AssetsPath[m_Type];
	assetPath += m_Path;

	if (Base::IsFileExists(assetPath.c_str()))
	{
		m_Root = assetPath.substr(0U, assetPath.length() - m_Path.length() - 1U);
		m_Path = assetPath;
		return true;
	}

	return false;
}

void AssetFile::GetAssetType()
{
	std::string ext = Base::GetFileExtension(m_Path, true);

	if (ext == ".shader")
	{
		m_Type = eShader;
	}
	else if (ext == ".bin")
	{
		m_Type = eShaderBinary;
	}
	else if (ext == ".dds" || ext == ".bmp")
	{
		m_Type = eTexture;
	}
	else if (ext == ".sdkmesh")
	{
		m_Type = eSDKMesh;
	}
	else if (ext == ".txt")
	{
		m_Type = eTxtMesh;
	}
	else if (ext == ".obj")
	{
		m_Type = eObjMesh;
	}
	else if (ext == ".mp3")
	{
		m_Type = eSound;
	}
	else if (ext == ".dat")
	{
		m_Type = eLevelData;
	}
	else
	{
		assert(!"Unknown asset type!");
	}
}

std::shared_ptr<uint8_t> AssetFile::Load()
{
	assert((eTypeCount != m_Type) && m_Size);

	if (m_Data != nullptr)
	{
		return m_Data;
	}

	int32_t readMode = std::ios::in;
	if (eTexture == m_Type || eShaderBinary == m_Type)
	{
		readMode |= std::ios::binary;
	}

	std::ifstream file(m_Path, readMode);
	assert(file.good());

	auto pMemory = new uint8_t[m_Size]();
	file.read((char *)pMemory, m_Size);
	file.close();

	m_Data.reset(pMemory);

	return m_Data;
}