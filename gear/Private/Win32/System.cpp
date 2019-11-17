#include "Gear/Public/Extension/System.h"
#include "Gear/Public/Extension/File.h"
#include "Gear/Public/Extension/Log.h"

#if defined(Platform_Win32)

namespaceStart(gear)

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
	verify_Log(::CreatePipe(&readHandle, &writeHandle, &securityAttr, INT16_MAX) != 0);
	verify_Log(::SetStdHandle(STD_OUTPUT_HANDLE, writeHandle) != 0);

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
					verify_Log(::ReadFile(readHandle, outputBuffer, INT16_MAX, &readBytes, nullptr) != 0);
					outputBuffer[readBytes] = '\0';
					bResult = false;
				}
			}
			else
			{
				verify_Log(0);
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

std::string getApplicationPath()
{
	char8_t appPath[MAX_PATH] = {};
	verify_Log(::GetModuleFileNameA(nullptr, appPath, MAX_PATH) != 0);
	return std::string(appPath);
}

std::string getApplicationName()
{
	std::string appName(getApplicationPath());
	appName = file::getFileName(appName);
	return appName;
}

void sleep(uint32_t microseconds)
{
	::Sleep(microseconds);
}

math::rect getWindowRect(uint64_t windowHandle)
{
	::RECT winRect;
	verify_Log(::GetWindowRect((::HWND)windowHandle, &winRect) != 0);

	return math::rect
	{
		(float32_t)winRect.left,
		(float32_t)winRect.top,
		(float32_t)winRect.right,
		(float32_t)winRect.bottom
	};
}

namespaceEnd(gear)

#endif