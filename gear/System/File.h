#pragma once

#include "gear/Public/Util.h"

namespaceStart(gear)

class file
{
public:
	struct folderTree
	{
		std::string Name;
		std::vector<std::shared_ptr<folderTree>> Children;
	};

	static bool isExists(const std::string &filePath);
	static bool isValidDirectory(const std::string &targetPath);
	static std::string getExtension(const std::string &filePath, bool bToLower = false);
	static std::string getName(const std::string &filePath, bool bToLower = false);
	static std::string getRootDirectory(const std::string &filePath, bool bToLower = false);
	static std::string getDirectory(const std::string &filePath, bool bToLower = false);
	static std::vector<std::string> getFileList(const std::string &targetPath, const std::vector<std::string> &filters, bool bToLower = false);
	static std::string findFile(const std::string &targetPath, const std::string &fileName);
	static folderTree getFolderTree(const std::string &targetPath, bool bToLower = false, bool bFullPath = false);
	static ::FILETIME getLastTime(const std::string &filePath);

	file() = default;
	file(const std::string &filePath);

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
protected:
private:
	size_t m_Size = 0ull;
	std::string m_Name;
	std::string m_Extension;
	std::string m_FullPath;
	std::string m_RelPath;
};

namespaceEnd(gear)
