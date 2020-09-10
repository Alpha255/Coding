#include "Gear/System.h"
#include "Gear/File.hpp"

NAMESPACE_START(Gear)

#if defined(PLATFORM_WIN32)

std::string System::getErrorMessage(uint32_t err)
{
	char8_t buffer[UINT16_MAX]{};
	VERIFY_SYSTEM(::FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		err == std::numeric_limits<uint32_t>().max() ? ::GetLastError() : err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buffer,
		UINT16_MAX,
		nullptr) != 0);

	return std::string(buffer);
}

std::string System::getWorkingDirectory()
{
	char8_t buffer[UINT16_MAX]{};
	VERIFY_SYSTEM(::GetCurrentDirectoryA(UINT16_MAX, buffer) != 0);
	return std::string(buffer);
}

void System::setWorkingDirectory(const std::string& path)
{
	assert(File::isExists(path, true));
	VERIFY_SYSTEM(::SetCurrentDirectoryA(path.c_str()) != 0);
}

void System::sleep(uint32_t microseconds)
{
	::Sleep(microseconds);
}

void System::executeProcess(const std::string& commandline, bool8_t waitDone)
{
	::SECURITY_ATTRIBUTES security
	{
		sizeof(::SECURITY_ATTRIBUTES),
		nullptr,
		true
	};

	::HANDLE read = nullptr, write = nullptr;
	VERIFY_SYSTEM(::CreatePipe(&read, &write, &security, INT16_MAX) != 0);
	VERIFY_SYSTEM(::SetStdHandle(STD_OUTPUT_HANDLE, write) != 0);

	/// If an error occurs, the ANSI version of this function (GetStartupInfoA) can raise an exception. 
	/// The Unicode version (GetStartupInfoW) does not fail
	::STARTUPINFOA startupInfo{};
	::GetStartupInfoA(&startupInfo);
	startupInfo.cb = sizeof(::STARTUPINFOA);
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	startupInfo.hStdInput = read;
	startupInfo.hStdOutput = write;

	::PROCESS_INFORMATION processInfo{};
	if (::CreateProcessA(
		nullptr,
		const_cast<::LPSTR>(commandline.c_str()),
		nullptr,
		nullptr,
		true,
		CREATE_NO_WINDOW,
		nullptr,
		nullptr,
		&startupInfo,
		&processInfo))
	{
		if (waitDone)
		{
			::DWORD exit = 0u;
			::WaitForSingleObject(processInfo.hProcess, INFINITE);
			if (::GetExitCodeProcess(processInfo.hProcess, &exit) && exit != 0u)
			{
				::DWORD bytes = 0u;
				char8_t buffer[INT16_MAX] = {};
				VERIFY_SYSTEM(::ReadFile(read, buffer, INT16_MAX, &bytes, nullptr) != 0);
				buffer[bytes] = '\0';
				LOG_ERROR("Failed to execute command \"%s\"", buffer);
			}
			else
			{
				VERIFY_SYSTEM(0);
			}

			/// STILL_ACTIVE
		}

		::CloseHandle(processInfo.hThread);
		::CloseHandle(processInfo.hProcess);

		::CloseHandle(read);
		::CloseHandle(write);

		return;
	}

	VERIFY_SYSTEM(0);
}

std::string System::getEnvironmentVariable(const std::string& var)
{
	char8_t buffer[UINT16_MAX]{};
	VERIFY_SYSTEM(::GetEnvironmentVariableA(var.c_str(), buffer, UINT16_MAX) != 0);
	return std::string(buffer);
}

uint64_t System::getModuleHandle()
{
	::HMODULE handle = ::GetModuleHandleA(nullptr);
	VERIFY_SYSTEM(handle);
	return reinterpret_cast<uint64_t>(handle);
}

System::DynamicLibrary::DynamicLibrary(const std::string& name)
{
	m_Handle = reinterpret_cast<uint64_t>(::LoadLibraryA(name.c_str()));
	VERIFY_SYSTEM(m_Handle);
}

void* System::DynamicLibrary::getProcAddress(const std::string& name)
{
	assert(m_Handle);
	return ::GetProcAddress(reinterpret_cast<::HMODULE>(m_Handle), name.c_str());
}

System::DynamicLibrary::~DynamicLibrary()
{
	VERIFY_SYSTEM(::FreeLibrary(reinterpret_cast<::HMODULE>(m_Handle)) != 0);
}

#else
	#error Unknown platform!
#endif

NAMESPACE_END(Gear)