#pragma once

#include <stdint.h>

class CommandLine
{
public:
	CommandLine() = default;
	~CommandLine() = default;

	static void Set(const wchar_t *pCmdLine)
	{

	}
protected:
	static void Parse();
	static void Run();
private:
};
