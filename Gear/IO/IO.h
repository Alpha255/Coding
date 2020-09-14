#pragma once

#include "Gear/File.hpp"

NAMESPACE_START(Gear)

class SyncContinuousOFStream : public File, public std::ofstream
{
public:
	SyncContinuousOFStream(const std::string& path, File::EMode mode = File::EMode::Text)
		: File(path)
		, std::ofstream(path, std::ios::out | (mode == File::EMode::Binary ? std::ios::binary : 0))
	{
	}

	template<class T>
	void write(const T& value)
	{
		assert(std::is_trivial<T>());
		std::ofstream::write(reinterpret_cast<const char8_t*>(&value), sizeof(T));
	}

	void write(const void* bytes, size_t size)
	{
		std::ofstream::write(reinterpret_cast<const char8_t*>(bytes), size);
	}

	void alignTo(size_t size)
	{

	}

	virtual ~SyncContinuousOFStream()
	{
		close();
	}
protected:
private:
	size_t m_Size = 0ull;
};

NAMESPACE_END(Gear)