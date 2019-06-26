#include "Base.h"

NamespaceBegin(Base)

inline void ToLower(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), 
		[](char c) 
	{ 
		return (char)::tolower((int32_t)c); 
	});
}

void Replace(std::string &str, const char src, const char dst)
{
	std::transform(str.begin(), str.end(), str.begin(),
		[src, dst](char c)
	{
		return (c == src ? dst : c);
	});
}

void Split(const std::string &src, const char token, std::vector<std::string> &result)
{
	std::string srcStr(src);
	size_t index = srcStr.find(token);

	while (index != std::string::npos)
	{
		std::string split = srcStr.substr(0U, index);
		if (split.length() > 0U)
		{
			result.emplace_back(split);
		}
		srcStr = srcStr.substr(index + 1U);
		index = srcStr.find(token);
	}

	if (srcStr.length() > 0U)
	{
		result.emplace_back(srcStr);
	}
}

std::string FormatString(const char *pStr, ...)
{
	std::unique_ptr<char> pResult = nullptr;
	if (nullptr != pStr)
	{
		va_list list = nullptr;
		va_start(list, pStr);
		size_t size = (size_t)_vscprintf(pStr, list) + 1;
		pResult = std::unique_ptr<char>(new char[size]());
		_vsnprintf_s(pResult.get(), size, size, pStr, list);
		va_end(list);
	}

	return std::string(pResult.get());
}

std::string GetFileName(const std::string &src)
{
	size_t index = src.rfind('/');
	if (index == std::string::npos)
	{
		index = src.rfind('\\');
	}

	return index == std::string::npos ? src : src.substr(index + 1U);
}

std::string GetFileExtension(const std::string &filePath, bool bToLower)
{
	size_t index = filePath.rfind(".");

	std::string ext("");
	if (index != std::string::npos)
	{
		ext = filePath.substr(index);
	}

	if (bToLower)
	{
		ToLower(ext);
	}

	return ext;
}

std::string GetRootDirectory(const std::string &path)
{
	size_t index = path.find("\\");
	if (index != std::string::npos)
	{
		index = path.find("\\", index + 1U);
		if (index != std::string::npos)
		{
			return path.substr(0U, index);
		}
	}

	return "";
}

std::string GetParentDirectory(const std::string &path)
{
	size_t index = path.rfind("\\");
	if (index != std::string::npos)
	{
		return path.substr(0U, index);
	}

	return "";
}

void BuildFileList(
	std::vector<std::string> &outFileList, 
	const std::string &targetPath, 
	const std::vector<std::string> &filters, 
	bool bToLower)
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

				BuildFileList(outFileList, subDir, filters, bToLower);
			}
			else
			{
				std::string fileName(findData.cFileName);

				if (filters.size() == 0U)
				{
					std::string filePath = targetPath + "\\" + fileName;
					if (bToLower)
					{
						ToLower(filePath);
					}

					outFileList.emplace_back(filePath);
				}
				else
				{
					std::string fileExt = GetFileExtension(fileName, true);
					for (auto it = filters.cbegin(); it != filters.cend(); ++it)
					{
						std::string filter = *it;
						ToLower(filter);

						if (fileExt == filter)
						{
							std::string filePath = targetPath + "\\" + fileName;
							if (bToLower)
							{
								ToLower(filePath);
							}

							outFileList.emplace_back(filePath);
							break;
						}
					}
				}
			}
		}

		if (!::FindNextFileA(hFileHandle, &findData))
		{
			break;
		}
	}
}

void BuildFolderTree(Tree<std::string> &outTree, const std::string &targetPath, bool bToLower, bool bFullPath)
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

				std::shared_ptr<Tree<std::string>> subTree(new Tree<std::string>);
				subTree->Name = bFullPath ? subDir : findData.cFileName;
				outTree.Children.emplace_back(subTree);

				BuildFolderTree(*subTree, subDir, bToLower, bFullPath);
			}
		}

		if (!::FindNextFileA(hFileHandle, &findData))
		{
			break;
		}
	}
}

bool IsValidDirectory(const std::string &targetPath)
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

bool IsFileExists(const std::string &filePath)
{
	uint32_t attri = ::GetFileAttributesA(filePath.c_str());

	return (attri != INVALID_FILE_ATTRIBUTES &&
		!(attri & FILE_ATTRIBUTE_DIRECTORY));
}

::FILETIME GetFileLastWriteTime(const std::string &filePath)
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

void Log(const char *pMessage, ...)
{
	std::unique_ptr<char> message = nullptr;
	if (nullptr != pMessage)
	{
		va_list list = nullptr;
		va_start(list, pMessage);
		size_t size = (size_t)_vscprintf(pMessage, list) + 1;
		message = std::unique_ptr<char>(new char[size]());
		_vsnprintf_s(message.get(), size, size, pMessage, list);
		va_end(list);
	}

	OutputDebugStringA(message.get());
	OutputDebugStringA("\n");
}

#if 0
inline void GetShaderCode(const char *pTag, std::string &rawCode)
{
	size_t beginIndex = rawCode.find(pTag);
	assert(beginIndex != std::string::npos);
	size_t endIndex = rawCode.find(pTag, beginIndex + strlen(pTag));
	assert(endIndex != std::string::npos);

	rawCode = std::string(rawCode.begin() + beginIndex + strlen(pTag), rawCode.begin() + endIndex);
}

std::string GetShaderCode(const char *pFileName, const char *pRenderTag, uint32_t shaderType)
{
	const char *s_ShaderTag[] =
	{
		"#VertexShader",
		"#HullShader",
		"#DomainShader",
		"#GeometryShader",
		"#PixelShader",
		"#ComputeShader",
	};

	assert(pFileName && pRenderTag && shaderType < _countof(s_ShaderTag));

	ResourceFile shaderFile(pFileName);
	auto pRawCode = std::unique_ptr<uint8_t>(shaderFile.Load());
	std::string rawCode((char *)pRawCode.get());

	GetShaderCode(pRenderTag, rawCode);

	GetShaderCode(s_ShaderTag[shaderType], rawCode);

	return rawCode;
}
#endif

NamespaceEnd(Base)