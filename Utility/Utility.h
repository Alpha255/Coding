#pragma once

#include "Common.h"
#include <string>

NamespaceBegin(Utility)

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

NamespaceEnd(Utility)
