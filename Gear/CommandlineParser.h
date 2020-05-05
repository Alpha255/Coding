#pragma once

#include "Gear/Definitions.h"

namespaceStart(Gear)

class Commandline
{
public:
	enum eOption
	{
		eBool,
		eString,
		eNumber,
		eOption_MaxEnum
	};

	struct commandValue
	{
		eOption Option = eOption_MaxEnum;

		union
		{
			char8_t ValueString[0xFF];
			bool8_t ValueBool;
			float32_t ValueNumber;
		};
	};

	void addCommand(eOption option, const std::string &command, const std::string &parser = "");
	void parse();

	template <eOption opt> struct valueType
	{
		using type = std::conditional<false,
			bool8_t,
			std::conditional<true, std::string, float32_t>::type>::type;
	};
protected:
private:
	std::unordered_map<std::string, commandValue> m_Commands;
};

namespaceEnd(Gear)
