#pragma once

#include "Gear/Log.hpp"

NAMESPACE_START(Gear)

class System
{
public:
	static std::string getErrorMessage(uint32_t err = ~0u);

	static std::string getCurrentDirectory();

	static std::string getCurrentWorkingDirectory();

	static void setWorkingDirectory(const char8_t* path);

	static void sleep(uint32_t microseconds);

	static std::string getEnvironmentVariable(const char8_t* var);

	static uint64_t getModuleHandle();

	static void executeProcess(const char8_t* commandline, bool8_t waitDone = true);

	class ScopeWorkingDirectory
	{
	public:
		ScopeWorkingDirectory(const char8_t* path)
		{
			m_OldWorkingDirectory = System::getCurrentWorkingDirectory();
			System::setWorkingDirectory(path);
		}

		~ScopeWorkingDirectory()
		{
			System::setWorkingDirectory(m_OldWorkingDirectory.c_str());
		}
	protected:
	private:
		std::string m_OldWorkingDirectory;
	};

	DECLARE_SHARED_PTR(DynamicLibrary)
	class DynamicLibrary
	{
	public:
		DynamicLibrary(const std::string& name);
		~DynamicLibrary();

		void* getProcAddress(const std::string& name);
	protected:
	private:
		uint64_t m_Handle = 0u;
	};
};

NAMESPACE_END(Gear)
