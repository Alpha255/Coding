#pragma once

#include "Gear/File.hpp"

NAMESPACE_START(Gear)

DECLARE_SHARED_PTR(Asset)
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
	AssetID m_ID = 0ull;  /// for feature use
};

NAMESPACE_END(Gear)
