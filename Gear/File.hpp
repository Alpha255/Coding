#pragma once

#include "Gear/System.h"

NAMESPACE_START(Gear)

struct FileTime
{
	uint16_t Year = 0u;
	uint16_t Month = 0u;
	uint16_t Day = 0u;
	uint16_t Hour = 0u;
	uint16_t Minutes = 0u;
	uint16_t Seconds = 0u;

	bool8_t operator==(const FileTime& other)
	{
		return Year == other.Year &&
			Month == other.Month &&
			Day == other.Day &&
			Hour == other.Hour &&
			Minutes == other.Hour &&
			Seconds == other.Seconds;
	}

	bool8_t operator!=(const FileTime& other)
	{
		return !(*this == other);
	}

	friend bool8_t operator==(const FileTime& left, const FileTime& right)
	{
		return left.Year == right.Year &&
			left.Month == right.Month &&
			left.Day == right.Day &&
			left.Hour == right.Hour &&
			left.Minutes == right.Hour &&
			left.Seconds == right.Seconds;
	}

	friend bool8_t operator!=(const FileTime& left, const FileTime& right)
	{
		return !(left == right);
	}
};

class File
{
public:
	enum class EMode : uint8_t
	{
		Text,
		Binary
	};

	File() = delete;
	File(const File& other) = default;
	virtual ~File() = default;

	File(const std::string& path)
		: m_FullPath(path)
	{
		String::replace(m_FullPath, "/", "\\");

		m_Extension = extension(m_FullPath, true);
		m_Name = name(m_FullPath);

		m_RelPath = m_FullPath.substr(m_FullPath.find("\\") + 1ull);
	}

	File(File&& other)
		: m_Size(other.m_Size)
		, m_Name(std::move(other.m_Name))
		, m_Extension(std::move(other.m_Extension))
		, m_FullPath(std::move(other.m_FullPath))
		, m_RelPath(std::move(other.m_RelPath))
		, m_Data(std::move(other.m_Data))
	{
		std::exchange(other.m_Size, {});
	}

	File& operator=(const File& other)
	{
		m_Size = other.m_Size;
		m_Name = other.m_Name;
		m_Extension = other.m_Extension;
		m_FullPath = other.m_FullPath;
		m_RelPath = other.m_RelPath;
		m_Data = other.m_Data;
		return *this;
	}

	File& operator=(File&& other)
	{
		m_Size = std::exchange(other.m_Size, {});
		m_Name.assign(std::move(other.m_Name));
		m_Extension.assign(std::move(other.m_Extension));
		m_FullPath.assign(std::move(other.m_FullPath));
		m_RelPath.assign(std::move(other.m_RelPath));
		m_Data = std::move(other.m_Data);
		LOG_INFO("Move");
		return *this;
	}

	static std::string name(const std::string& path, bool8_t lowercase = false)
	{
		std::string result;

		size_t index = path.rfind('/');
		if (index == std::string::npos)
		{
			index = path.rfind('\\');
		}

		if (index != std::string::npos)
		{
			result = path.substr(index + 1ull);
		}

		return lowercase ? String::lowercase(result) : result;
	}

	static std::string nameWithoutExtension(const std::string& path, bool8_t lowercase = false)
	{
		std::string nameWithExt = name(path, lowercase);
		std::string ext = extension(nameWithExt, lowercase);
		return nameWithExt.substr(0u, nameWithExt.length() - ext.length() - 1ull);
	}

	static std::string extension(const std::string& path, bool8_t lowercase = false)
	{
		size_t index = path.rfind(".");

		std::string result;
		if (index != std::string::npos)
		{
			result = path.substr(index);
		}

		return lowercase ? String::lowercase(result) : result;
	}

	static std::string directory(const std::string& path, bool8_t relative = false, bool8_t lowercase = false)
	{
		std::string result(path);
		String::replace(result, "/", "\\");

		size_t index = result.rfind("\\");
		if (index != std::string::npos)
		{
			result = result.substr(0ull, index);
		}

		if (relative && (index = result.find("\\")) != std::string::npos)
		{
			result = result.substr(index + 1ull);
		}

		return lowercase ? String::lowercase(result) : result;
	}

	static FileTime lastWriteTime(const std::string& path);
	static bool8_t isExists(const std::string& path, bool8_t isDirectory = false);
	static size_t size(const std::string& path);

	static std::vector<std::string> getFileList(const std::string& path, const std::vector<std::string>& filters, bool8_t lowercase = false);
	static std::string find(const std::string& path, const std::string& name);
	static void createDirectory(const std::string& path);

	inline std::string name() const
	{
		return m_Name;
	}

	inline std::string extension() const
	{
		return m_Extension;
	}

	inline std::string fullPath() const
	{
		return m_FullPath;
	}

	inline std::string relativePath() const
	{
		return m_RelPath;
	}

	size_t size()
	{
		assert(isExists(m_FullPath));
		if (!m_Size)
		{
			m_Size = size(m_FullPath);
		}

		return m_Size;
	}

	std::shared_ptr<byte8_t> data(EMode mode = EMode::Text)
	{
		if (!m_Data)
		{
			int32_t readMode = mode == EMode::Binary ? std::ios::in | std::ios::binary : std::ios::in;

			std::ifstream fs(m_FullPath, readMode);
			assert(fs.is_open());

			m_Data.reset(new byte8_t[size()]());
			fs.read((char8_t*)m_Data.get(), m_Size);
			fs.close();
		}

		return m_Data;
	}
protected:
private:
	size_t m_Size = 0ull;
	std::string m_Name;
	std::string m_Extension;
	std::string m_FullPath;
	std::string m_RelPath;

	std::shared_ptr<byte8_t> m_Data = nullptr;
};

NAMESPACE_END(Gear)
