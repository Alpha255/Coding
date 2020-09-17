#pragma once

#include "Gear/Log.hpp"

NAMESPACE_START(Gear)

class System
{
public:
	static std::string getErrorMessage(uint32_t err = std::numeric_limits<uint32_t>().max());

	static std::string getCurrentDirectory();

	static std::string getCurrentWorkingDirectory();

	static void setWorkingDirectory(const std::string& path);

	static void sleep(uint32_t microseconds);

	static std::string getEnvironmentVariable(const std::string& var);

	static uint64_t getModuleHandle();

	static void executeProcess(const std::string& commandline, bool8_t waitDone = true);

	class ScopeWorkingDirectory
	{
	public:
		ScopeWorkingDirectory(const std::string& path)
		{
			m_OldWorkingDirectory = System::getCurrentWorkingDirectory();
			System::setWorkingDirectory(path);
		}

		~ScopeWorkingDirectory()
		{
			System::setWorkingDirectory(m_OldWorkingDirectory);
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

#if defined(PLATFORM_WIN32)
	#define RUN_APPLICATION(ClassName)                                 \
		int32_t WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int32_t) \
		{                                                              \
			Gear::Application::Configs DefaultConfig;                  \
			ClassName App_##ClassName;                                 \
			App_##ClassName.initialize(#ClassName, DefaultConfig);     \
			App_##ClassName.run();                                     \
		}
#else
	#error Unknown platform!
#endif
