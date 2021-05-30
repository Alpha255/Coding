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
		assert(is_open());
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
		(void)size;
	}

	virtual ~SyncContinuousOFStream()
	{
		close();
	}
protected:
private:
};

class SyncContinuousIFStream : public File, public std::ifstream
{
public:
	SyncContinuousIFStream(const std::string& path, File::EMode mode = File::EMode::Text)
		: File(path)
		, std::ifstream(path, std::ios::in | (mode == File::EMode::Binary ? std::ios::binary : 0))
	{
		assert(is_open());
	}

	template<class T>
	T read()
	{
		assert(std::is_trivial<T>());
		VERIFY((m_ReadSize + sizeof(T)) <= File::size());

		T data{};
		std::ifstream::read(reinterpret_cast<char8_t*>(&data), sizeof(T));
		m_ReadSize += sizeof(T);

		return data;
	}

	template<class T, size_t Size>
	std::array<T, Size> read()
	{
		assert(std::is_trivial<T>());
		VERIFY((m_ReadSize + sizeof(T) * Size) <= File::size());

		std::array<T, Size> data{};
		std::ifstream::read(reinterpret_cast<char8_t*>(data.data()), sizeof(T) * Size);
		m_ReadSize += (sizeof(T) * Size);

		return data;
	}

	std::shared_ptr<byte8_t> readBytes(size_t size)
	{
		VERIFY((m_ReadSize + size) <= File::size());

		std::shared_ptr<byte8_t> bytes(new byte8_t[size]());
		std::ifstream::read(reinterpret_cast<char8_t*>(bytes.get()), size);
		m_ReadSize += size;

		return bytes;
	}

	virtual ~SyncContinuousIFStream()
	{
		close();
	}
protected:
private:
	size_t m_ReadSize = 0ull;
};

NAMESPACE_END(Gear)