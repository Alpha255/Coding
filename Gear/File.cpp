#include "Gear/Gear.h"

namespaceStart(Gear)

File::File(const std::string &filePath)
{
	if (!isExists(filePath))
	{
		return;
	}

	m_FullPath = filePath;
	replace(m_FullPath, '/', '\\');

	m_Extension = extension(m_FullPath, true);
	m_Name = name(m_FullPath);

	std::string rootPath(rootDirectory(m_FullPath));
	m_RelPath = m_FullPath.substr(rootPath.length() + 1ull);

	std::ifstream reader(filePath, std::ios::in);
	assert(reader.is_open());
	reader.seekg(0u, std::ios::end);
	m_Size = (size_t)reader.tellg();
	reader.close();
}

std::string File::extension(const std::string &filePath, bool8_t bToLower)
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

std::string File::name(const std::string &filePath, bool8_t bToLower)
{
	assert(extension(filePath, false).length() > 0u);
	std::string name(filePath);

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

std::string File::rootDirectory(const std::string &filePath, bool8_t bToLower)
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

std::string File::directory(const std::string &filePath, bool8_t bToLower)
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

std::string File::stripExtension(const std::string &filePath, bool8_t bToLower)
{
	std::string result(filePath);
	std::string ext = extension(filePath, bToLower);
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

void File::read(eReadAs readAs)
{
	int32_t mode = readAs == eBinary ? std::ios::in | std::ios::binary : std::ios::in;

	std::ifstream fileStream(fullPath(), mode);
	assert(fileStream.is_open());

	auto dataSize = size();
	m_Data.reset(new byte8_t[dataSize]());
	fileStream.read((char8_t *)m_Data.get(), dataSize);
	fileStream.close();
}

namespaceEnd(Gear)
