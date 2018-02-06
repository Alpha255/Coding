#include "CommandLine.h"

#include <direct.h>

bool CommandLine::s_Inited = false;
char CommandLine::s_InFileDir[MAX_PATH] = {};
char CommandLine::s_OutFileDir[MAX_PATH] = {};
std::vector<std::string> CommandLine::s_StatList;

bool CommandLine::IsValidDir(const char *pDir)
{
	::WIN32_FIND_DATAA fileData = {};
	bool bValid = false;

	::HANDLE fileHandle = ::FindFirstFileA(pDir, &fileData);
	if ((fileHandle != INVALID_HANDLE_VALUE) && (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		bValid = true;
	}
	::FindClose(fileHandle);

	return bValid;
}

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
		else if (0 == strcmp(ppArgv[i], "--StatList"))
		{
			++i;
			while (i < argc)
			{
				if (strncmp(ppArgv[i], "--", 2U) != 0)
				{
					s_StatList.push_back(ppArgv[i]);
				}
				else
				{
					break;
				}
				++i;
			}
		}
		else
		{
			assert(!"Unrecognized command!!!");
		}
	}

	assert(IsValidDir(s_InFileDir));

	if (0U == strnlen_s(s_OutFileDir, MAX_PATH))
	{
		strcpy_s(s_OutFileDir, s_InFileDir);
	}
	else if (!IsValidDir(s_OutFileDir))
	{
		if (::_mkdir(s_OutFileDir) != 0)
		{
			assert(!"Failed to create file directory!!!!");
		}
	}

	s_Inited = true;
}