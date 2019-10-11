#include "File.h"

namespaceStart(gear)

file::file(const std::string &filePath)
{
	if (!isExists(filePath))
	{
		return;
	}

	m_FullPath = filePath;
	replace(m_FullPath, '/', '\\');
	toLower(m_FullPath);

	m_Extension = getExtension(m_FullPath);
	m_Name = getName(m_FullPath);

	std::string rootPath(getRootDirectory(m_FullPath));
	m_RelPath = m_FullPath.substr(rootPath.length() + 1ull);

	::HANDLE fileHandle = ::CreateFileA(
		m_FullPath.c_str(),
		0,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		nullptr);
	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		::LARGE_INTEGER result{};
		verifyWin(::GetFileSizeEx(fileHandle, &result) != 0);
		m_Size = (size_t)result.QuadPart;
	}
	::CloseHandle(fileHandle);
}

bool8_t file::isExists(const std::string &filePath)
{
	uint32_t attri = ::GetFileAttributesA(filePath.c_str());

	return (attri != INVALID_FILE_ATTRIBUTES &&
		!(attri & FILE_ATTRIBUTE_DIRECTORY));
}

std::string file::getExtension(const std::string &filePath, bool8_t bToLower)
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

std::string file::getName(const std::string &filePath, bool8_t bToLower)
{
	assert(getExtension(filePath, false).length() > 0u);
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

std::string file::getRootDirectory(const std::string &filePath, bool8_t bToLower)
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

std::string file::getDirectory(const std::string &filePath, bool8_t bToLower)
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

::FILETIME file::getLastTime(const std::string & filePath)
{
	::FILETIME time = {};

	::HANDLE fileHandle = ::CreateFileA(
		filePath.c_str(),
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED | FILE_FLAG_BACKUP_SEMANTICS,
		nullptr);

	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		if (::GetFileTime(fileHandle, nullptr, nullptr, &time) != 0)
		{
			::CloseHandle(fileHandle);
			return time;
		}
	}

	::CloseHandle(fileHandle);

	return time;
}

std::string file::stripExtension(const std::string &filePath, bool8_t bToLower)
{
	std::string result(filePath);
	std::string ext = getExtension(filePath, bToLower);
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

namespaceEnd(gear)
