#pragma once

#include "Gear/Definitions.h"

NAMESPACE_START(Gear)

class UniqueID
{
public:
	UniqueID(const char8_t* name)
		: m_Name(name)
	{
	}

	UniqueID(std::string&& name)
		: m_Name(std::move(name))
	{
	}
protected:
private:
	uint32_t m_ID = std::numeric_limits<uint32_t>().max();
	std::string m_Name;
};

NAMESPACE_END(Gear)