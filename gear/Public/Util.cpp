#include "Util.h"

namespaceStart(gear)

void toLower(std::string &srcStr)
{
	std::transform(srcStr.begin(), srcStr.end(), srcStr.begin(),
		[](char c)
	{
		return (char)::tolower((int32_t)c);
	});
}

void replace(std::string &srcStr, char8_t srcC, char8_t dstC)
{
	std::replace(srcStr.begin(), srcStr.end(), srcC, dstC);
}

template <typename Token> std::vector<std::string> split(const std::string &srcStr, const Token &token)
{
	std::vector<std::string> result;

	std::string temp(srcStr);
	size_t index = temp.find(token);

	while (index != std::string::npos)
	{
		std::string split = temp.substr(0u, index);
		if (split.length() > 0u)
		{
			result.emplace_back(split);
		}
		temp = temp.substr(index + 1U);
		index = temp.find(token);
	}

	if (temp.length() > 0U)
	{
		result.emplace_back(temp);
	}

	return result;
}

std::vector<std::string> split(const std::string &srcStr, char8_t token)
{
	return split<char8_t>(srcStr, token);
}

std::vector<std::string> split(const std::string &srcStr, const std::string &token)
{
	return split<std::string>(srcStr, token);
}

std::string format(const char8_t *pArgStr, ...)
{
	std::unique_ptr<char8_t> pResult = nullptr;
	if (pArgStr)
	{
		va_list list = nullptr;
		va_start(list, pArgStr);
		size_t size = (size_t)_vscprintf(pArgStr, list) + 1;
		pResult = std::unique_ptr<char8_t>(new char8_t[size]());
		_vsnprintf_s(pResult.get(), size, size, pArgStr, list);
		va_end(list);
	}

	return std::string(pResult.get());
}

void log(const char8_t *pMessage, ...) 
{
	std::unique_ptr<char8_t> message = nullptr;
	if (pMessage)
	{
		va_list list = nullptr;
		va_start(list, pMessage);
		size_t size = (size_t)_vscprintf(pMessage, list) + 1;
		message = std::unique_ptr<char8_t>(new char8_t[size]());
		_vsnprintf_s(message.get(), size, size, pMessage, list);
		va_end(list);
	}

	::OutputDebugStringA(message.get());
	::OutputDebugStringA("\n");
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
					gear::log("Shader compile failed: %s", outputBuffer);
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

namespaceEnd(gear)