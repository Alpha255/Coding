#pragma once

#include "Gear/Public/Definitions.h"

namespaceStart(gear)

void toLower(std::string &srcStr);

void replace(std::string &srcStr, const char8_t srcC, char8_t dstC);

std::vector<std::string> split(const std::string &srcStr, char8_t token);

std::vector<std::string> split(const std::string &srcStr, const std::string &token);

std::string format(const char8_t *pArgStr, ...);

namespaceEnd(gear)


