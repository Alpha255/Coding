#include "CommandLine.h"
#include "StatFileConverter.h"

#include <intrin.h>

int main(int argc, const char *argv[])
{
	CommandLine::Parse(argc, argv);

	StatFileConverter converter;
	converter.BuildStatFileList(CommandLine::GetInFileDir());

	converter.DoConvert();

	return 0;
}