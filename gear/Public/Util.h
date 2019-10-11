#pragma once

#include "gear/Public/Commandline.h"
#include "gear/Public/String.h"
#include "gear/Public/Singleton.h"

namespaceStart(gear)

template <typename Left, typename Right>
inline bool isEqual(const Left &left, const Right &right)
{
	assert(sizeof(Left) == sizeof(Right));
	return ::memcmp(&left, &right, sizeof(Right)) == 0;
}

struct folderTree
{
	std::string Name;
	std::vector<std::shared_ptr<folderTree>> Children;
};

bool8_t isValidDirectory(const std::string &targetPath);

std::vector<std::string> getFileList(const std::string &targetPath, const std::vector<std::string> &filters, bool8_t bToLower = false);

std::string findFile(const std::string &targetPath, const std::string &fileName);

folderTree getFolderTree(const std::string &targetPath, bool8_t bToLower = false, bool8_t bFullPath = false);

void log(const char8_t *pMessage, ...);

bool8_t executeProcess(const std::string &commandline, bool8_t bWaitUntilDone = true);

namespaceEnd(gear)


