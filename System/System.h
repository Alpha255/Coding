#pragma once

#include "Common.h"
#include <string>
#include <vector>

NamespaceBegin(System)

enum eResourceType
{
	eShader,
	eTexture,
	eSDKMesh,
	eTxtMesh,
	eObjMesh,
	eSound,
	eRawData,
	eResTypeCount
};

std::string ResourceFileDirectory(eResourceType resType);

std::string ResourceFilePath(const char *pFileName, eResourceType resType);

bool IsStrEndwith(const std::string &srcStr, const char *pPostfix);

void GetFileListFromDirectory(std::vector<std::string> &outFileList, const char *pTargetDir, const char *pFilter = nullptr);

bool IsValidDirectory(const char *pTargetDir);

NamespaceEnd(System)
