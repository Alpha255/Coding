#include "Gear/CommandlineParser.h"

namespaceStart(Gear)

void Commandline::addCommand(eOption, const std::string &, const std::string &)
{
	valueType<eString>::type type = "Test";
}

void Commandline::parse()
{
}

namespaceEnd(Gear)
