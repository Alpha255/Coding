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
	eResTypeCount
};

std::string ResourceFileDirectory(eResourceType resType);

inline std::string ResourceFilePath(const char *pFileName, eResourceType resType, const char *pSecondLevelDir = nullptr)
{
	std::string path = ResourceFileDirectory(resType);

	if (pSecondLevelDir)
	{
		path += pSecondLevelDir;
	}

	path += pFileName;

	return path;
}

NamespaceEnd(Utility)
