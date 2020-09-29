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

	const VertexAttributes& vertexLayout() const
	{
		return m_VertexLayout;
	}

	bool8_t isDirty() const
	{
		return false;
	}

	friend struct MaterialWriter;
protected:
	void reload();

	void serialize(const std::string& path);

	void deserialize(const std::string& path);
private:
	std::string m_Name;
	VertexAttributes m_VertexLayout;
};

struct MaterialWriter
{
	Material Target;
};

NAMESPACE_END(Gfx)
