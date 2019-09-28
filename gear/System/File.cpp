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
		verify(::GetFileSize(fileHandle, (::LPDWORD)(&m_Size)) != INVALID_FILE_SIZE);
	}
	::CloseHandle(fileHandle);
}

void buildFileList(
	std::vector<std::string> &outFileList,
	const std::string &targetPath,
	const std::vector<std::string> &filters,
	bool bToLower)
{
	std::string rootDir = targetPath + "\\*.*";

	::WIN32_FIND_DATAA findData = {};
	::HANDLE fileHandle = ::FindFirstFileA(rootDir.c_str(), &findData);

	while (true)
	{
		if (findData.cFileName[0] != '.')
		{
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::string subDir = targetPath + "\\" + findData.cFileName;

				buildFileList(outFileList, subDir, filters, bToLower);
			}
			else
			{
				std::string fileName(findData.cFileName);

				if (filters.size() == 0U)
				{
					std::string filePath = targetPath + "\\" + fileName;
					if (bToLower)
					{
						toLower(filePath);
					}

					outFileList.emplace_back(filePath);
				}
				else
				{
					std::string fileExt = file::getExtension(fileName, true);
					for (auto it = filters.cbegin(); it != filters.cend(); ++it)
					{
						std::string filter = *it;
						toLower(filter);

						if (fileExt == filter)
						{
							std::string filePath = targetPath + "\\" + fileName;
							if (bToLower)
							{
								toLower(filePath);
							}

							outFileList.emplace_back(filePath);
							break;
						}
					}
				}
			}
		}

		if (!::FindNextFileA(fileHandle, &findData))
		{
			break;
		}
	}
}

std::string tryToFindFile(
	const std::string &targetPath,
	const std::string &fileName)
{
	std::string rootDir = targetPath + "\\*.*";

	::WIN32_FIND_DATAA findData = {};
	::HANDLE fileHandle = ::FindFirstFileA(rootDir.c_str(), &findData);

	while (true)
	{
		if (findData.cFileName[0] != '.')
		{
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				tryToFindFile(targetPath + "\\" + findData.cFileName, fileName);
			}
			else
			{
				if (stricmp(findData.cFileName, fileName.c_str()) == 0)
				{
					return targetPath + "\\" + findData.cFileName;
				}
			}
		}

		if (!::FindNextFileA(fileHandle, &findData))
		{
			break;
		}
	}

	return std::string();
}

void buildFolderTree(file::folderTree &outTree, const std::string &targetPath, bool bToLower, bool bFullPath)
{
	std::string rootDir = targetPath + "\\*.*";

	::WIN32_FIND_DATAA findData = {};
	::HANDLE hFileHandle = ::FindFirstFileA(rootDir.c_str(), &findData);

	while (true)
	{
		if (findData.cFileName[0] != '.')
		{
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::string subDir = targetPath + "\\" + findData.cFileName;

				std::shared_ptr<file::folderTree> subTree(new file::folderTree);
				subTree->Name = bFullPath ? subDir : findData.cFileName;
				outTree.Children.emplace_back(subTree);

				buildFolderTree(*subTree, subDir, bToLower, bFullPath);
			}
		}

		if (!::FindNextFileA(hFileHandle, &findData))
		{
			break;
		}
	}
}

bool file::isExists(const std::string &filePath)
{
	uint32_t attri = ::GetFileAttributesA(filePath.c_str());

	return (attri != INVALID_FILE_ATTRIBUTES &&
		!(attri & FILE_ATTRIBUTE_DIRECTORY));
}

bool file::isValidDirectory(const std::string &targetPath)
{
	::WIN32_FIND_DATAA fileData = {};
	bool bValid = false;

	::HANDLE fileHandle = ::FindFirstFileA(targetPath.c_str(), &fileData);
	if ((fileHandle != INVALID_HANDLE_VALUE) && (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		bValid = true;
	}
	::FindClose(fileHandle);

	return bValid;
}

std::string file::getExtension(const std::string &filePath, bool bToLower)
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

std::string file::getName(const std::string &filePath, bool bToLower)
{
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

std::string file::getRootDirectory(const std::string &filePath, bool bToLower)
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

std::string file::getDirectory(const std::string &filePath, bool bToLower)
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

std::vector<std::string> file::getFileList(const std::string &targetPath, const std::vector<std::string>& filters, bool bToLower)
{
	if (!isValidDirectory(targetPath))
	{
		return std::vector<std::string>();
	}

	std::vector<std::string> result;
	buildFileList(result, targetPath, filters, bToLower);

	return result;
}

std::string file::findFile(const std::string &targetPath, const std::string &fileName)
{
	if (!isValidDirectory(targetPath))
	{
		return std::string();
	}

	return tryToFindFile(targetPath, fileName);
}

file::folderTree file::getFolderTree(const std::string &targetPath, bool bToLower, bool bFullPath)
{
	if (!isValidDirectory(targetPath))
	{
		return folderTree();
	}

	folderTree result;
	buildFolderTree(result, targetPath, bToLower, bFullPath);

	return result;
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

namespaceEnd(gear)
