#include "Utility.h"

NamespaceBegin(Utility)

std::string ResourceFileDirectory(eResourceType resType)
{
	static const char* s_ResourcePath[eResourceType::eResTypeCount] =
	{
		"\\Resource\\Shaders\\",
		"\\Resource\\Textures\\",
		"\\Resource\\SDKMeshs\\",
		"\\Resource\\TxtMeshs\\",
		"\\Resource\\ObjMeshs\\",
	};
	static char directory[MAX_PATH]{};
	::GetModuleFileNameA(::GetModuleHandle(nullptr), directory, MAX_PATH);

	std::string appDir(directory);
	std::string resFileDir = appDir.substr(0, appDir.rfind("\\"));
	resFileDir += s_ResourcePath[resType];

	return resFileDir;
}

NamespaceEnd(Utility)