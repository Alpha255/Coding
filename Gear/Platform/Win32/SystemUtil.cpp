#include "Gear/Gear.h"

#if defined(Platform_Win32)

namespaceStart(Gear)

std::string getErrorMessage(uint32_t errorCode)
{
	std::string message;

	char8_t msg[UINT16_MAX] = {};
	verify(::FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		msg,
		UINT16_MAX,
		nullptr) != 0);
	if (strlen(msg) > 0)
	{
		message = std::string(msg);
	}

	return message;
}

bool8_t executeProcess(const std::string &Commandline, bool8_t bWaitUntilDone)
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
		(::LPSTR)Commandline.c_str(),
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
					Logger::instance().log(Logger::eError, std::string(outputBuffer));
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

void sleep(uint32_t microseconds)
{
	::Sleep(microseconds);
}

Math::Rect getWindowRect(uint64_t windowHandle)
{
	::RECT winRect{};
	verify_Log(::GetWindowRect((::HWND)windowHandle, &winRect) != 0);

	return Math::Rect
	{
		(float32_t)winRect.left,
		(float32_t)winRect.top,
		(float32_t)winRect.right,
		(float32_t)winRect.bottom
	};
}

std::string getEnvironmentVariable(const std::string &envVarName)
{
	char8_t result[UINT16_MAX] = {};
	verify_Log(::GetEnvironmentVariableA(envVarName.c_str(), result, UINT16_MAX) != 0);

	return result;
}

uint64_t getAppInstance()
{
	::HMODULE hInst = ::GetModuleHandleA(nullptr);
	verify_Log(hInst);
	return (uint64_t)hInst;
}

Math::Vec2 getDesktopWindowSize()
{
#if 0
	uint32_t width = ::GetSystemMetrics(SM_CXFULLSCREEN);
	uint32_t height = ::GetSystemMetrics(SM_CYFULLSCREEN);

	return math::vec2((float32_t)width, (float32_t)height);
#else
	::HWND hWnd = ::GetDesktopWindow();
	verify_Log(hWnd != nullptr);

	::RECT rect{};
	::GetWindowRect(hWnd, &rect);

	return Math::Vec2((float32_t)(rect.right - rect.left), (float32_t)(rect.bottom - rect.top));
#endif
}

void DynamicLibrary::load(const std::string &libraryName)
{
	::HMODULE result = ::LoadLibraryA(libraryName.c_str());
	if (result != nullptr)
	{
		Handle = (uint64_t)result;
	}
	assert(result);
}

void *DynamicLibrary::getProcAddress(const std::string &procName)
{
	assert(Handle);
	void *result = ::GetProcAddress((::HMODULE)Handle, procName.c_str());
	///verify_Log(result);
	return result;
}

void DynamicLibrary::free()
{
	if (Handle != 0u)
	{
		verify_Log(::FreeLibrary((::HMODULE)Handle) != 0);
	}
}

namespaceEnd(Gear)

#endif