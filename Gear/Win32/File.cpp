#include "Gear/File.hpp"

NAMESPACE_START(Gear)

#if defined(PLATFORM_WIN32)

size_t File::size(const std::string& path)
{
	size_t size = 0u;
	if (isExists(path))
	{
#if defined(PLATFORM_WIN32)
		::HANDLE handle = ::CreateFileA(
			path.c_str(),
			0,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			nullptr);
		if (handle != INVALID_HANDLE_VALUE)
		{
			::LARGE_INTEGER result{};
			VERIFY_SYSTEM(::GetFileSizeEx(handle, &result) != 0);
			size = static_cast<size_t>(result.QuadPart);
			::CloseHandle(handle);
		}
#else
		std::ifstream fs(path, std::ios::in);
		assert(fs.is_open());
		fs.seekg(0u, std::ios::end);
		size = static_cast<size_t>(reader.tellg());
		fs.close();
#endif
	}

	return size;
	}

bool8_t File::isExists(const std::string& path, bool8_t isDirectory)
{
#if 1
	uint32_t attr = ::GetFileAttributesA(path.c_str());
	return (attr != INVALID_FILE_ATTRIBUTES && isDirectory ? (attr & FILE_ATTRIBUTE_DIRECTORY) : !(attr & FILE_ATTRIBUTE_DIRECTORY));
#else
	::WIN32_FIND_DATAA findData{};
	::HANDLE fileHandle = ::FindFirstFileA(path.c_str(), &findData);
	bool8_t exists = (fileHandle != INVALID_HANDLE_VALUE) && isDirectory ? (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) : true;
	::FindClose(fileHandle);

	return exists;
#endif
}

FileTime File::lastWriteTime(const std::string& path)
{
	::FILETIME time{};
	::HANDLE handle = ::CreateFileA(
		path.c_str(),
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED | FILE_FLAG_BACKUP_SEMANTICS,
		nullptr);

	if (handle != INVALID_HANDLE_VALUE)
	{
		VERIFY_SYSTEM(::GetFileTime(handle, nullptr, nullptr, &time) != 0);
		::CloseHandle(handle);

		::SYSTEMTIME sysTime{};
		VERIFY_SYSTEM(::FileTimeToSystemTime(&time, &sysTime) != 0);

		return FileTime
		{
			sysTime.wYear,
			sysTime.wMonth,
			sysTime.wDay,
			sysTime.wHour,
			sysTime.wMinute,
			sysTime.wSecond
		};
	}

	return FileTime();
}

void makeFileList(
	__out std::vector<std::string>& result,
	const std::string& path,
	const std::vector<std::string>& filters,
	bool8_t lowercase)
{
	std::string root(path + "\\*.*");

	::WIN32_FIND_DATAA findData{};
	::HANDLE handle = ::FindFirstFileA(root.c_str(), &findData);

	while (true)
	{
		if (findData.cFileName[0] != '.')
		{
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::string subDir = path + "\\" + findData.cFileName;

				makeFileList(result, subDir, filters, lowercase);
			}
			else
			{
				std::string fileName(findData.cFileName);

				if (filters.size() == 0u)
				{
					std::string filePath = path + "\\" + fileName;
					result.push_back(lowercase ? std::move(String::lowercase(filePath)) : std::move(filePath));
				}
				else
				{
					std::string ext = File::extension(fileName, true);
					for (auto it = filters.cbegin(); it != filters.cend(); ++it)
					{
						std::string filter(*it);
						String::toLower(filter);

						if (ext == filter)
						{
							std::string filePath = path + "\\" + fileName;
							result.push_back(lowercase ? std::move(String::lowercase(filePath)) : std::move(filePath));
							break;
						}
					}
				}
			}
		}

		if (!::FindNextFileA(handle, &findData))
		{
			break;
		}
	}
}

void tryToFindFile(const std::string& path, const std::string& name, __out std::string& result)
{
	std::string root(path + "\\*.*");

	::WIN32_FIND_DATAA findData{};
	::HANDLE handle = ::FindFirstFileA(root.c_str(), &findData);

	while (true)
	{
		if (findData.cFileName[0] != '.')
		{
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				tryToFindFile(path + "\\" + findData.cFileName, name, result);
			}
			else
			{
				if (_stricmp(findData.cFileName, name.c_str()) == 0)
				{
					result = path + "\\" + findData.cFileName;
					break;
				}
			}
		}

		if (!::FindNextFileA(handle, &findData))
		{
			break;
		}
	}
}

std::vector<std::string> File::getFileList(const std::string& path, const std::vector<std::string>& filters, bool8_t lowercase)
{
	std::vector<std::string> result;

	if (File::isExists(path))
	{
		makeFileList(result, path, filters, lowercase);
	}

	return result;
}

std::string File::find(const std::string& path, const std::string& name)
{
	std::string result;
	if (File::isExists(path, true))
	{
		tryToFindFile(path, name, result);
	}

	return result;
}

void File::createDirectory(const std::string& path)
{
	std::wstring LPath(path.cbegin(), path.cend());
	VERIFY_SYSTEM(ERROR_SUCCESS == ::SHCreateDirectory(nullptr, LPath.c_str()));
}

#else
	#error Unknown platform!
#endif

NAMESPACE_END(Gear)
