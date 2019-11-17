#pragma once

#include "Gear/Public/Definitions.h"
#include "Gear/Public/Independent/Math/Rect.h"

namespaceStart(gear)

template <typename Left, typename Right>
inline bool8_t isEqual(const Left &left, const Right &right)
{
	assert(sizeof(Left) == sizeof(Right));
	return ::memcmp(&left, &right, sizeof(Right)) == 0;
}

std::string getErrorMessage(uint32_t errorCode);

std::string getApplicationPath();

std::string getApplicationName();

bool8_t executeProcess(const std::string &commandline, bool8_t bWaitUntilDone = true);

void sleep(uint32_t microseconds);

math::rect getWindowRect(uint64_t windowHandle);

namespaceEnd(gear)
