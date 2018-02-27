#include "CommandLine.h"

#include "Utility.h"

bool CommandLine::s_Inited = false;
char CommandLine::s_InFileDir[MAX_PATH] = {};
char CommandLine::s_OutFileDir[MAX_PATH] = {};
char CommandLine::s_UERootDir[MAX_PATH] = {};

void CommandLine::Parse(uint32_t argc, const char **ppArgv)
{
	assert(ppArgv);

	for (uint32_t i = 1U; i < argc; ++i)
	{
		if (0 == strcmp(ppArgv[i], "--InFileDir"))
		{
			strcpy_s(s_InFileDir, ppArgv[i + 1]);
			++i;
		}
		else if (0 == strcmp(ppArgv[i], "--OutFileDir"))
		{
			strcpy_s(s_OutFileDir, ppArgv[i + 1]);
			++i;
		}
		else if (0 == strcmp(ppArgv[i], "--UERootDir"))
		{
			strcpy_s(s_UERootDir, ppArgv[i + 1]);
			++i;
		}
		else
		{
			assert(!"Unrecognized command!!!");
		}
	}

	assert(Utility::IsValidDirectory(s_InFileDir));
	assert(Utility::IsValidDirectory(s_UERootDir));

	if (0U == strnlen_s(s_OutFileDir, MAX_PATH))
	{
		strcpy_s(s_OutFileDir, s_InFileDir);
	}

	///assert(Utility::IsValidDirectory(s_OutFileDir));

	s_Inited = true;
}