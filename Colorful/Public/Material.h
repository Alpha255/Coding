#pragma once

#include "Colorful/Public/GfxResource.h"
#include "AssetTool/Asset.h"

DeclareShared(Material)
class Material : public Gear::File
{
public:
	Material() = default;
	Material(const std::string& name);

	inline std::string name() const
	{
		return m_Name;
	}

	bool8_t isDirty() const
	{
		return false;
	}
protected:
	void reload();
	void serialize();
	void deserialize();
private:
	std::string m_Name;
	std::vector<GfxVertexAttributes> m_InputLayout;
	GfxShaderPtr m_Shaders[eRShaderUsage_MaxEnum];
};
