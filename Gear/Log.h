#pragma once

#include "Gear/Singleton.h"

NAMESPACE_START(Gear)

class Log
{
public:
	template<class... Args>
	static void logError(const char8_t* fmt, Args&&... args)
	{
		log(ELevel::Error, fmt, std::forward<Args>(args)...);
	}

	template<class... Args>
	static void logInfo(const char8_t* fmt, ...)
	{
		log(ELevel::Info, fmt, std::forward<Args>(args)...);
	}

	template<class... Args>
	static void logWarning(const char8_t* fmt, ...)
	{
		log(ELevel::Warning, fmt, std::forward<Args>(args)...);
	}
protected:
	enum class ELevel
	{
		Info,
		Warning,
		Error
	};

	static void log(ELevel level, const char8_t* fmt, ...)
	{

	}
private:
};

NAMESPACE_END(Gear)
