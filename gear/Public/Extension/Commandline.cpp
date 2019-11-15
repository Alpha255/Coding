#include "Commandline.h"

namespaceStart(gear)

void commandline::addCommand(eOption, const std::string &, const std::string &)
{
	valueType<eString>::type type = "Test";
}

void commandline::parse()
{
}

namespaceEnd(gear)
