#pragma once

#include "Colorful/Public/GfxResource.h"

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(Material)
class EXPORT_API Material /// Just for test
{
public:
	Material(const std::string& name);

	std::string name() const
	{
		return m_Name;
	}

	bool8_t isDirty() const
	{
		return false;
	}
protected:
	void reload();

	void serialize(const std::string& path);

	void deserialize(const std::string& path);
private:
	std::string m_Name;
};

NAMESPACE_END(Gfx)
