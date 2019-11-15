#pragma once

#include "Gear/Public/Independent/Singleton.h"

namespaceStart(gear)

class logger : public singleton<logger>
{
	singletonDeclare(logger);
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

namespaceEnd(gear)