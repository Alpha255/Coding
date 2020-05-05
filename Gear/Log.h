#pragma once

#include "Gear/Singleton.h"

namespaceStart(Gear)

class Logger : public Singleton<Logger>
{
	singletonDeclare(Logger);
public:
	enum eLogLevel
	{
		eInfo,
		eWarning,
		eError,
		eLogLevel_MaxEnum
	};

	enum eLogTaget
	{
		eToOutput,
		eToStd,
		eToFile,
		eLogTarget_MaxEnum
	};

	void log(eLogLevel level, const char8_t *pMessage, ...);

	inline void log(eLogLevel level, const std::string &message)
	{
		log(level, message.c_str());
	}

	inline void redirectTarget(eLogTaget target)
	{
		m_LogTarget = target;
	}

	inline void renameLogFile(const std::string &logFileName)
	{
		m_LogFileName = logFileName;
	}
protected:
private:
	eLogTaget m_LogTarget = eToOutput;
	std::string m_LogFileName{ "gearLog.txt" };
};

namespaceEnd(Gear)
