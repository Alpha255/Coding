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

	static inline const char *GetUERootDir()
	{
		assert(s_Inited);
		return s_UERootDir;
	}
protected:
private:
	static char s_InFileDir[MAX_PATH];
	static char s_OutFileDir[MAX_PATH];
	static char s_UERootDir[MAX_PATH];
	static bool s_Inited;
};
