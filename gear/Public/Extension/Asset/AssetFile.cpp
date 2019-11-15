#include "AssetFile.h"

namespaceStart(gear)

void assetFile::matchType()
{
	/// Need to move into config file
	std::string ext = getExtension();

	if (ext == ".dds" || ext == ".bmp")
	{
		m_Type = eTexture;
	}
	else if (ext == ".shader")
	{
		m_Type = eShader;
	}
	else if (ext == ".sdkmesh" || ext == ".txt" || ext == ".obj" || ext == ".dae")
	{
		m_Type = eStaticMesh;
	}
	else if (ext == ".mp3")
	{
		m_Type = eSound;
	}
	else if (ext == ".dat")
	{
		m_Type = eLevel;
	}
}

namespaceEnd(gear)
