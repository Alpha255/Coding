#pragma once

#include <stdint.h>
#include <Windows.h>
#include <assert.h>
#include <vector>

class CommandLine
{
public:
	CommandLine() = default;
	~CommandLine() = default;

	static void Parse(uint32_t argc, const char **ppArgv);

	static inline const char *GetInFileDir()
	{
		assert(s_Inited);
		return s_InFileDir;
	}

	static inline const char *GetOutFileDir()
	{
		assert(s_Inited);
		return s_OutFileDir;
	}
protected:
	static bool IsValidDir(const char *pDir);
private:
	static char s_InFileDir[MAX_PATH];
	static char s_OutFileDir[MAX_PATH];
	static bool s_Inited;
	static std::vector<std::string> s_StatList;
};
