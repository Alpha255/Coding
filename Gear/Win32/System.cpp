#include "Gear/System.h"
#include "Gear/File.hpp"

NAMESPACE_START(Gear)

#if defined(PLATFORM_WIN32)

std::string System::getErrorMessage(uint32_t err)
{
	static std::shared_ptr<char8_t> buffer(new char8_t[UINT16_MAX]());
	memset(buffer.get(), 0, UINT16_MAX);

	VERIFY(::FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		err == ~0u ? ::GetLastError() : err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buffer.get(),
		UINT16_MAX,
		nullptr) != 0);

	return std::string(buffer.get());
}

std::string System::getCurrentDirectory()
{
	static std::shared_ptr<char8_t> buffer(new char8_t[UINT16_MAX]());
	memset(buffer.get(), 0, UINT16_MAX);

	VERIFY_SYSTEM(::GetModuleFileNameA(nullptr, buffer.get(), UINT16_MAX) != 0);
	return File::directory(std::string(buffer.get()));
}

std::string System::getCurrentWorkingDirectory()
{
	static std::shared_ptr<char8_t> buffer(new char8_t[UINT16_MAX]());
	memset(buffer.get(), 0, UINT16_MAX);

	VERIFY_SYSTEM(::GetCurrentDirectoryA(UINT16_MAX, buffer.get()) != 0);
	return std::string(buffer.get());
}

void System::setWorkingDirectory(const char8_t* path)
{
	assert(File::isExists(path, true));
	VERIFY_SYSTEM(::SetCurrentDirectoryA(path) != 0);
}

void System::sleep(uint32_t microseconds)
{
	::Sleep(microseconds);
}

void System::executeProcess(const char8_t* commandline, bool8_t waitDone)
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
	::STARTUPINFOA startupInfo;
	::GetStartupInfoA(&startupInfo);
	startupInfo.cb = sizeof(::STARTUPINFOA);
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	startupInfo.hStdInput = read;
	startupInfo.hStdOutput = write;

	static std::shared_ptr<char8_t> buffer(new char8_t[INT16_MAX]());
	memset(buffer.get(), 0, INT16_MAX);

	::PROCESS_INFORMATION processInfo;
	if (::CreateProcessA(
		nullptr,
		const_cast<::LPSTR>(commandline),
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
				VERIFY_SYSTEM(::ReadFile(read, buffer.get(), INT16_MAX, &bytes, nullptr) != 0);
				//buffer[bytes] = '\0';
				LOG_ERROR("Failed to execute command \"%s\"", buffer.get());
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

std::string System::getEnvironmentVariable(const char8_t* var)
{
	static std::shared_ptr<char8_t> buffer(new char8_t[UINT16_MAX]());
	memset(buffer.get(), 0, UINT16_MAX);

	VERIFY_SYSTEM(::GetEnvironmentVariableA(var, buffer.get(), UINT16_MAX) != 0);
	return std::string(buffer.get());
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