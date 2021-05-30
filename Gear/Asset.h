#pragma once

#include "Gear/File.hpp"

NAMESPACE_START(Gear)

class Asset : public File
{
public:
	using AssetID = uint32_t;
	using File::File;

	Asset(const std::string& path)
		: File(path)
	{
	}

	enum class ECategory : uint8_t
	{
		Unknown,
		Texture,
		Model,
		Shader,
		Material,
		Audio
	};

	inline ECategory caterogy() const
	{
		return m_Category;
	}

	inline AssetID ID() const
	{
		return m_ID;
	}
protected:
private:
	ECategory m_Category = ECategory::Unknown;
	AssetID m_ID = 0u;  /// for feature use
};
DECLARE_SHARED_PTR(Asset)

NAMESPACE_END(Gear)
