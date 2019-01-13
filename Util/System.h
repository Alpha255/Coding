#pragma once

#include "Common.h"
#include <string>
#include <vector>

NamespaceBegin(System)

bool IsStrEndwith(const char *pStr, const char *pPostfix);

void GetFileListFromDirectory(std::vector<std::string> &outFileList, const char *pTargetDir, const char *pFilter = nullptr);

bool IsValidDirectory(const char *pTargetDir);

bool FileExists(const char *pFilePath);

void Log(const char *pFormatMsg, ...);

std::string GetShaderCode(const char *pFileName, const char *pRenderTag, uint32_t shaderType);

NamespaceEnd(System)
