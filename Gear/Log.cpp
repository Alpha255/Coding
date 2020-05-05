#include "Log.h"

namespaceStart(Gear)

void Logger::log(eLogLevel level, const char8_t *pMessage, ...)
{
	/// Need thread safe log
	assert(level < eLogLevel_MaxEnum);

	static std::string s_LogLevel[eLogLevel_MaxEnum] =
	{
		"Info: ",
		"Warning: ",
		"Error: "
	};

	std::string logMessage(s_LogLevel[level]);

	if (pMessage)
	{
		va_list list = nullptr;
		va_start(list, pMessage);
		size_t size = (size_t)_vscprintf(pMessage, list) + 1;
		auto message = std::make_unique<char8_t[]>(size);
		_vsnprintf_s(message.get(), size, size, pMessage, list);
		va_end(list);

		logMessage += message.get();
	}

	logMessage += "\n";

	if (eToOutput == m_LogTarget)
	{
#if defined(Platform_Win32)
		::OutputDebugStringA(logMessage.c_str());
#else
#endif
	}
	else if (eToStd == m_LogTarget)
	{
		std::cout << logMessage << std::endl;
	}
	else if (eToFile == m_LogTarget)
	{
		assert(0);
	}
}

namespaceEnd(Gear)
