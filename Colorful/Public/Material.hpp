#pragma once

#include "Colorful/Public/GfxResource.h"
#include "AssetTool/AssetTool.h"
#include "Gear/IO/IO.h"

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(Material)
class Material
{
public:
	Material(const std::string& name)
	{
		auto asset = AssetTool::instance().findAsset(name);
		assert(asset);

		deserialize(asset->fullPath());
	}

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
	void reload()
	{

	}

	void serialize(const std::string& path)
	{

	}

	void deserialize(const std::string& path)
	{
		SyncContinuousIFStream is(path, File::EMode::Binary);

		uint32_t nameLength = is.read<uint32_t>();
		if (nameLength > 0u)
		{
			auto bytes = is.readBytes(nameLength);
			m_Name = std::string(reinterpret_cast<const char8_t*>(bytes.get()));
		}
		else
		{
			m_Name = "Unknown Material";
		}
	}
private:
	std::string m_Name;
	VertexAttributes m_VertexLayout;
};

struct MaterialWriter
{
	Material Target;
};

NAMESPACE_END(Gfx)
