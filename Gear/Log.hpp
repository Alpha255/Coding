#pragma once

#include "Gear/Singleton.h"
#include "Gear/String.hpp"

NAMESPACE_START(Gear)

class Log
{
public:
	enum class ELevel
	{
		Info,
		Warning,
		Error
	};

	static void log(ELevel level, const std::string& message)
	{
#if defined(PLATFORM_WIN32)
		std::string logMessage(s_LevelInfo[static_cast<size_t>(level)]);
		logMessage += message;
		logMessage += "\n";
		::OutputDebugStringA(logMessage.c_str());
#else
		assert(0);
#endif
	}
protected:
	static constexpr const char8_t* const s_LevelInfo[]
	{
		"Info: ",
		"Warning: ",
		"Error: "
	};
private:
};

#define LOG_INFO(Format, ...)    Gear::Log::log(Gear::Log::ELevel::Info, Gear::String::format(Format, __VA_ARGS__))
#define LOG_WARNING(Format, ...) Gear::Log::log(Gear::Log::ELevel::Warning, Gear::String::format(Format, __VA_ARGS__))
#define LOG_ERROR(Format, ...)   Gear::Log::log(Gear::Log::ELevel::Error, Gear::String::format(Format, __VA_ARGS__))

NAMESPACE_END(Gear)
