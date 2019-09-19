#pragma once

#include "Common.h"

namespaceStart(gear)

class noneCopyable
{
public:
	noneCopyable() = default;
	~noneCopyable() = default;

	noneCopyable(const noneCopyable &) = delete;
	void operator=(const noneCopyable &) = delete;
protected:
private:
};

template <typename Left, typename Right>
inline bool isEqual(const Left &left, const Right &right)
{
	assert(sizeof(Left) == sizeof(Right));
	return ::memcmp(&left, &right, sizeof(Right)) == 0;
}

void toLower(std::string &srcStr);

void replace(std::string &srcStr, const char srcC, const char dstC);

std::vector<std::string> split(const std::string &srcStr, const char token);

std::vector<std::string> split(const std::string &srcStr, const std::string &token);

std::string format(const char *pArgStr, ...);

void log(const char *pMessage, ...);

namespaceEnd(gear)


