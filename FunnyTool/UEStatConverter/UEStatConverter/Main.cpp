#include "CommandLine.h"
#include "StatFileConverter.h"

int main(int argc, const char *argv[])
{
	CommandLine::Parse(argc, argv);

	StatFileConverter converter;
	converter.BuildStatFileList();

	converter.DoConvert();

	return 0;
}