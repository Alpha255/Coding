#include "AssetFile.h"
#include "AssetManager.h"

AssetFile::AssetFile(const std::string &fileName)
	: m_Name(fileName)
{
	std::string name(fileName);
	Base::ToLower(name);
	m_Path = AssetManager::Instance().Get(name);

	assert(m_Path.length() > 0U);
	m_Root = Base::GetParentDirectory(m_Path);
	GetAssetType();

	std::ifstream file(m_Path, eTexture == m_Type ? std::ios::in | std::ios::binary : std::ios::in);
	assert(file.good());

	file.seekg(0U, std::ios::end);
	m_Size = (size_t)file.tellg();
	file.close();
}

void AssetFile::GetAssetType()
{
	std::string ext = Base::GetFileExtension(m_Name, true);

	if (ext == ".shader" || ext == ".vert" || ext == ".frag" || ext == ".hlsl")
	{
		m_Type = eShader;
	}
	else if (ext == ".bin" || ext == ".spv")
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
		assert(0);
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
	if (eTexture == m_Type || eShaderBinary == m_Type || eSDKMesh == m_Type)
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