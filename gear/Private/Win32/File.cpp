#include "Gear/Public/Extension/File.h"
#include "Gear/Public/Extension/System.h"
#include "Gear/Public/Extension/Log.h"
#include "Gear/Public/Independent/String.h"

#if defined(Platform_Win32)

namespaceStart(gear)

size_t file::getFileSize(const std::string &filePath)
{
	size_t size = 0u;
	if (isFileExists(filePath))
	{
		::HANDLE fileHandle = ::CreateFileA(
			filePath.c_str(),
			0,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			nullptr);
		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			::LARGE_INTEGER result{};
			verify_Log(::GetFileSizeEx(fileHandle, &result) != 0);
			size = (size_t)result.QuadPart;
			::CloseHandle(fileHandle);
		}
	}

	return size;
}

bool8_t file::isFileExists(const std::string &filePath)
{
	uint32_t attri = ::GetFileAttributesA(filePath.c_str());

	return (attri != INVALID_FILE_ATTRIBUTES &&
		!(attri & FILE_ATTRIBUTE_DIRECTORY));
}

bool8_t file::isDirectoryExists(const std::string &targetPath)
{
	::WIN32_FIND_DATAA fileData = {};
	bool8_t bValid = false;

	::HANDLE fileHandle = ::FindFirstFileA(targetPath.c_str(), &fileData);
	if ((fileHandle != INVALID_HANDLE_VALUE) && (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		bValid = true;
	}
	::FindClose(fileHandle);

	return bValid;
}

file::fileTime file::getFileLastWriteTime(const std::string & filePath)
{
	::FILETIME time = {};
	fileTime result = {};

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
		verify_Log(::GetFileTime(fileHandle, nullptr, nullptr, &time) != 0);
		::CloseHandle(fileHandle);

		::SYSTEMTIME sysTime = {};
		verify_Log(::FileTimeToSystemTime(&time, &sysTime) != 0);

		result = fileTime
		{
			(uint32_t)sysTime.wYear,
			(uint32_t)sysTime.wMonth,
			(uint32_t)sysTime.wDay,
			(uint32_t)sysTime.wHour,
			(uint32_t)sysTime.wMinute,
			(uint32_t)sysTime.wSecond
		};
	}

	return result;
}

void buildFileList(
	std::vector<std::string> &outFileList,
	const std::string &targetPath,
	const std::vector<std::string> &filters,
	bool8_t bToLower)
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
					std::string fileExt = file::getFileExtension(fileName, true);
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

void tryToFindFile(
	const std::string &targetPath,
	const std::string &fileName,
	std::string &outFilePath)
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
				tryToFindFile(targetPath + "\\" + findData.cFileName, fileName, outFilePath);
			}
			else
			{
				if (_stricmp(findData.cFileName, fileName.c_str()) == 0)
				{
					outFilePath = targetPath + "\\" + findData.cFileName;
					break;
				}
			}
		}

		if (!::FindNextFileA(fileHandle, &findData))
		{
			break;
		}
	}
}

void buildFolderTree(file::folderTree &outTree, const std::string &targetPath, bool8_t bToLower, bool8_t bFullPath)
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

std::vector<std::string> file::getFileList(const std::string &targetPath, const std::vector<std::string> &filters, bool8_t bToLower)
{
	if (!file::isDirectoryExists(targetPath))
	{
		return std::vector<std::string>();
	}

	std::vector<std::string> result;
	buildFileList(result, targetPath, filters, bToLower);

	return result;
}

std::string file::findFile(const std::string &targetPath, const std::string &fileName)
{
	std::string filePath;
	if (file::isDirectoryExists(targetPath))
	{
		tryToFindFile(targetPath, fileName, filePath);
	}

	return filePath;
}

file::folderTree file::getFolderTree(const std::string &targetPath, bool8_t bToLower, bool8_t bFullPath)
{
	if (!file::isDirectoryExists(targetPath))
	{
		return folderTree();
	}

	folderTree result;
	buildFolderTree(result, targetPath, bToLower, bFullPath);

	return result;
}

void file::createDirectory(const std::string &directory)
{
	std::wstring wDirectory(directory.cbegin(), directory.cend());
	verify_Log(ERROR_SUCCESS == ::SHCreateDirectory(nullptr, wDirectory.c_str()));
}

namespaceEnd(gear)

#endif
