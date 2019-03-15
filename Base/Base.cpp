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

void BuildFileList(
	std::vector<std::string> &outFileList, 
	const std::string &targetPath, 
	const std::vector<std::string> filters, 
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
				std::string fileExt = GetFileExtension(fileName, true);

				for (auto it = filters.begin(); it != filters.end(); ++it)
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

void Log(const char *pFormatMsg, ...)
{
	static const size_t s_MsgLength = MAX_PATH * 5U;
	static char message[s_MsgLength] = {};

	va_list ap = nullptr;
	va_start(ap, pFormatMsg);
	_vsnprintf_s(message, s_MsgLength, pFormatMsg, ap);
	va_end(ap);

	strcat_s(message, "\n");
	OutputDebugStringA(message);
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