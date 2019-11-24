#pragma once

#include "Gear/Public/Definitions.h"

namespaceStart(gear)

class file
{
public:
	struct fileTime
	{
		uint32_t Year = 0u;
		uint32_t Month = 0u;
		uint32_t Day = 0u;
		uint32_t Hour = 0u;
		uint32_t Minutes = 0u;
		uint32_t Seconds = 0u;
	};

	struct folderTree
	{
		std::string Name;
		std::vector<std::shared_ptr<folderTree>> Children;
	};

	enum eReadAs
	{
		eText,
		eBinary
	};

	static bool8_t isDirectoryExists(const std::string &targetPath);
	static bool8_t isFileExists(const std::string &filePath);
	static size_t getFileSize(const std::string &filePath);
	static std::string getFileExtension(const std::string &filePath, bool8_t bToLower = false);
	static std::string getFileName(const std::string &filePath, bool8_t bToLower = false);
	static std::string getFileRootDirectory(const std::string &filePath, bool8_t bToLower = false);
	static std::string getFileDirectory(const std::string &filePath, bool8_t bToLower = false);
	static fileTime getFileLastWriteTime(const std::string &filePath);
	static std::string stripFileExtension(const std::string &filePath, bool8_t bToLower = false);
	static std::vector<std::string> getFileList(const std::string &targetPath, const std::vector<std::string> &filters, bool8_t bToLower = false);
	static std::string findFile(const std::string &targetPath, const std::string &fileName);
	static folderTree getFolderTree(const std::string &targetPath, bool8_t bToLower = false, bool8_t bFullPath = false);
	static void createDirectory(const std::string &directory);

	file() = default;
	file(const std::string &filePath);

	inline size_t getSize() const
	{
		return m_Size;
	}

	inline std::string getName() const
	{
		return m_Name;
	}

	inline std::string getExtension() const
	{
		return m_Extension;
	}

	inline std::string getFullPath() const
	{
		return m_FullPath;
	}

	inline std::string getRelativePath() const
	{
		return m_RelPath;
	}

	inline std::shared_ptr<byte> getData(eReadAs readAs = eText)
	{
		if (!m_Data)
		{
			read(readAs);
		}

		return m_Data;
	}
protected:
	void read(eReadAs readAs = eText);
private:
	size_t m_Size = 0ull;
	std::string m_Name;
	std::string m_Extension;
	std::string m_FullPath;
	std::string m_RelPath;

	std::shared_ptr<byte> m_Data;
};

namespaceEnd(gear)
