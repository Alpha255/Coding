#include "Util.h"
#include "gear/System/File.h"

namespaceStart(gear)

void log(eLogLevel level, const char8_t *pMessage, ...)
{
	assert(level < eLogLevel_MaxEnum);

	static std::string s_LogLevel[eLogLevel_MaxEnum] = 
	{
		"Info: ",
		"Warning: ",
		"Error: "
	};

	std::string logMessage(s_LogLevel[level]);

	std::unique_ptr<char8_t> message = nullptr;
	if (pMessage)
	{
		va_list list = nullptr;
		va_start(list, pMessage);
		size_t size = (size_t)_vscprintf(pMessage, list) + 1;
		message = std::unique_ptr<char8_t>(new char8_t[size]());
		_vsnprintf_s(message.get(), size, size, pMessage, list);
		va_end(list);

		logMessage += message.get();
	}

	logMessage += "\n";

	::OutputDebugStringA(logMessage.c_str());
}

std::string getErrorMessage(uint32_t errorCode)
{
	std::string message;

	::LPSTR msg = nullptr;
	::FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		nullptr,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		msg,
		0u,
		nullptr);
	if (msg)
	{
		message = std::string(msg);
	}

	return message;
}

bool8_t executeProcess(const std::string &commandline, bool8_t bWaitUntilDone)
{
	bool8_t bResult = true;
	::SECURITY_ATTRIBUTES securityAttr{};
	securityAttr.bInheritHandle = true;
	securityAttr.nLength = sizeof(::SECURITY_ATTRIBUTES);

	::HANDLE readHandle = nullptr, writeHandle = nullptr;
	verifyWin(::CreatePipe(&readHandle, &writeHandle, &securityAttr, INT16_MAX) != 0);
	verifyWin(::SetStdHandle(STD_OUTPUT_HANDLE, writeHandle) != 0);

	/// If an error occurs, the ANSI version of this function (GetStartupInfoA) can raise an exception. 
	/// The Unicode version (GetStartupInfoW) does not fail
	::STARTUPINFOA startupInfo{};
	::GetStartupInfoA(&startupInfo);
	startupInfo.cb = sizeof(::STARTUPINFOA);
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	startupInfo.hStdInput = readHandle;
	startupInfo.hStdOutput = writeHandle;

	::PROCESS_INFORMATION processInfo{};
	if (::CreateProcessA(
		nullptr, 
		(::LPSTR)commandline.c_str(), 
		nullptr, 
		nullptr, 
		true, 
		CREATE_NO_WINDOW,
		nullptr, 
		nullptr, 
		&startupInfo,
		&processInfo))
	{
		if (bWaitUntilDone)
		{
			::DWORD exitCode = 0u;
			::WaitForSingleObject(processInfo.hProcess, INFINITE);
			if (::GetExitCodeProcess(processInfo.hProcess, &exitCode))
			{
				if (exitCode != 0u)
				{
					::DWORD readBytes = 0u;
					char8_t outputBuffer[INT16_MAX] = {};
					verifyWin(::ReadFile(readHandle, outputBuffer, INT16_MAX, &readBytes, nullptr) != 0);
					outputBuffer[readBytes] = '\0';
					gear::log(gear::eError, "Shader compile failed: %s", outputBuffer);
					bResult = false;
				}
			}
			else
			{
				verifyWin(0);
			}

			/// STILL_ACTIVE
		}

		::CloseHandle(processInfo.hThread);
		::CloseHandle(processInfo.hProcess);

		::CloseHandle(readHandle);
		::CloseHandle(writeHandle);
	}

	return bResult;
}

bool8_t isValidDirectory(const std::string &targetPath)
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

void buildFolderTree(folderTree &outTree, const std::string &targetPath, bool8_t bToLower, bool8_t bFullPath)
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

				std::shared_ptr<folderTree> subTree(new folderTree);
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

std::vector<std::string> getFileList(const std::string &targetPath, const std::vector<std::string>& filters, bool8_t bToLower)
{
	if (!isValidDirectory(targetPath))
	{
		return std::vector<std::string>();
	}

	std::vector<std::string> result;
	buildFileList(result, targetPath, filters, bToLower);

	return result;
}

std::string findFile(const std::string &targetPath, const std::string &fileName)
{
	std::string filePath;
	if (isValidDirectory(targetPath))
	{
		tryToFindFile(targetPath, fileName, filePath);
	}

	return filePath;
}

folderTree getFolderTree(const std::string &targetPath, bool8_t bToLower, bool8_t bFullPath)
{
	if (!isValidDirectory(targetPath))
	{
		return folderTree();
	}

	folderTree result;
	buildFolderTree(result, targetPath, bToLower, bFullPath);

	return result;
}

namespaceEnd(gear)