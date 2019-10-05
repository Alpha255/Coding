#include "AssetFile.h"

namespaceStart(gear)

void assetFile::getType()
{
	std::string extension = getExtension();

	if (extension == ".dds" || extension == ".bmp")
	{
		m_Type = eTexture;
	}
	else if (extension == ".shader")
	{
		m_Type = eShader;
	}
	else if (extension == ".sdkmesh" || extension == ".txt" || extension == ".obj")
	{
		m_Type = eStaticMesh;
	}
	else if (extension == ".mp3")
	{
		m_Type = eSound;
	}
	else if (extension == ".dat")
	{
		m_Type = eLevel;
	}
}

namespaceEnd(gear)
