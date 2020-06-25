#pragma once

#include "Gear/Definitions.h"

namespaceStart(Gear)

class File
{
public:
	struct FileTime
	{
		uint32_t Year = 0u;
		uint32_t Month = 0u;
		uint32_t Day = 0u;
		uint32_t Hour = 0u;
		uint32_t Minutes = 0u;
		uint32_t Seconds = 0u;
	};

	struct FolderTree
	{
		std::string Name;
		std::vector<std::shared_ptr<FolderTree>> Children;
	};

	enum eFileMode
	{
		eText,
		eBinary
	};

	static std::string extension(const std::string &filePath, bool8_t bToLower = false);
	static std::string name(const std::string &filePath, bool8_t bToLower = false);
	static std::string rootDirectory(const std::string &filePath, bool8_t bToLower = false);
	static std::string directory(const std::string &filePath, bool8_t bToLower = false);
	static std::string stripExtension(const std::string &filePath, bool8_t bToLower = false);

	/// Platform specific
	static FileTime lastWriteTime(const std::string &filePath);
	static bool8_t isDirectoryExists(const std::string &targetPath);
	static bool8_t isExists(const std::string &filePath);
	static size_t size(const std::string &filePath);
	static std::vector<std::string> buildFileList(const std::string &targetPath, const std::vector<std::string> &filters, bool8_t bToLower = false);
	static std::string find(const std::string &targetPath, const std::string &fileName);
	static FolderTree buildFolderTree(const std::string &targetPath, bool8_t bToLower = false, bool8_t bFullPath = false);
	static void createDirectory(const std::string &directory);

	File() = default;
	File(const std::string &filePath);

	inline size_t size() const
	{
		return m_Size;
	}

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

	inline std::shared_ptr<byte> data(eFileMode readAs = eText)
	{
		if (!m_Data)
		{
			read(readAs);
		}

		return m_Data;
	}

	void read(eFileMode readAs = eText);
	void write(eFileMode writeAs)
	{
		write(writeAs, m_FullPath, data().get(), m_Size);
	}
	static void write(eFileMode wirteAs, const std::string& path, const byte8_t* content, size_t size);
protected:
private:
	size_t m_Size = 0ull;
	std::string m_Name;
	std::string m_Extension;
	std::string m_FullPath;
	std::string m_RelPath;

	std::shared_ptr<byte> m_Data;
};

namespaceEnd(Gear)
