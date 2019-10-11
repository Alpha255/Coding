#pragma once

#include "gear/Public/Common.h"

namespaceStart(gear)

class commandline
{
public:
	enum eOption
	{
		eBool,
		eString
	};

	void addCommand(eOption option, const std::string &command, const std::string &parser = "");
	void parse();
protected:
private:
	std::unordered_map<std::string, std::string> m_Commands;
};

namespaceEnd(gear)
