#include "CommandLine.h"
#include "StatFileConverter.h"

int main(int argc, const char *argv[])
{
	CommandLine::Parse(argc, argv);

	StatFileConverter converter;
	converter.DoConvert();

	///system("pause");

	return 0;
}