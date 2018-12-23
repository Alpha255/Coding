#pragma once

#include "Common.h"
#include <string>
#include <vector>

NamespaceBegin(System)

bool IsStrEndwith(const char *pStr, const char *pPostfix);

void GetFileListFromDirectory(std::vector<std::string> &outFileList, const char *pTargetDir, const char *pFilter = nullptr);

bool IsValidDirectory(const char *pTargetDir);

bool FileExists(const char *pFilePath);

NamespaceEnd(System)
