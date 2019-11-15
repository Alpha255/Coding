#include "File.h"
#include "Gear/Public/Independent/String.h"
#include "Gear/Private/Win32/File.hpp"

namespaceStart(gear)

file::file(const std::string &filePath)
{
	if (!isFileExists(filePath))
	{
		return;
	}

	m_FullPath = filePath;
	replace(m_FullPath, '/', '\\');
	toLower(m_FullPath);

	m_Extension = getFileExtension(m_FullPath);
	m_Name = getFileName(m_FullPath);

	std::string rootPath(getFileRootDirectory(m_FullPath));
	m_RelPath = m_FullPath.substr(rootPath.length() + 1ull);

	std::ifstream reader(filePath, std::ios::in);
	assert(reader.is_open());
	reader.seekg(0u, std::ios::end);
	m_Size = (size_t)reader.tellg();
	reader.close();
}

std::string file::getFileExtension(const std::string &filePath, bool8_t bToLower)
{
	size_t index = filePath.rfind(".");

	std::string ext("");
	if (index != std::string::npos)
	{
		ext = filePath.substr(index);
	}

	if (bToLower)
	{
		toLower(ext);
	}

	return ext;
}

std::string file::getFileName(const std::string &filePath, bool8_t bToLower)
{
	assert(getFileExtension(filePath, false).length() > 0u);
	std::string name("");

	size_t index = filePath.rfind('/');
	if (index == std::string::npos)
	{
		index = filePath.rfind('\\');
	}

	if (index != std::string::npos)
	{
		name = filePath.substr(index + 1U);
	}

	if (bToLower)
	{
		toLower(name);
	}

	return name;
}

std::string file::getFileRootDirectory(const std::string &filePath, bool8_t bToLower)
{
	std::string path("");

	std::string temp(filePath);
	replace(temp, '/', '\\');

	size_t index = temp.find("\\");
	if (index != std::string::npos)
	{
		path = temp.substr(0U, index);
	}

	if (bToLower)
	{
		toLower(path);
	}

	return path;
}

std::string file::getFileDirectory(const std::string &filePath, bool8_t bToLower)
{
	std::string path("");

	std::string temp(filePath);
	replace(temp, '/', '\\');

	size_t index = temp.rfind("\\");
	if (index != std::string::npos)
	{
		path = temp.substr(0U, index);
	}

	if (bToLower)
	{
		toLower(path);
	}

	return path;
}

std::string file::stripFileExtension(const std::string &filePath, bool8_t bToLower)
{
	std::string result(filePath);
	std::string ext = getFileExtension(filePath, bToLower);
	if (ext.length() > 0u)
	{
		result = result.substr(0u, result.length() - ext.length());
	}

	if (bToLower)
	{
		toLower(result);
	}

	return result;
}

void file::read(eReadAs readAs)
{
	m_Data = std::make_shared<byte>();

	int32_t mode = readAs == eBinary ? std::ios::in | std::ios::binary : std::ios::in;

	std::ifstream fileStream(getFullPath(), mode);
	assert(fileStream.is_open());

	byte *pMemory = new byte[getSize()]();
	fileStream.read((char8_t *)pMemory, getSize());
	fileStream.close();

	m_Data.reset(pMemory);
}

namespaceEnd(gear)
