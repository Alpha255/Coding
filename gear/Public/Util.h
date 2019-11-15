#pragma once

#include "Gear/Public/Commandline.h"
#include "Gear/Public/String.h"
#include "Gear/Public/Singleton.h"

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

enum eLogLevel
{
	eInfo,
	eWarning,
	eError,
	eLogLevel_MaxEnum
};

bool8_t isValidDirectory(const std::string &targetPath);

std::vector<std::string> getFileList(const std::string &targetPath, const std::vector<std::string> &filters, bool8_t bToLower = false);

std::string findFile(const std::string &targetPath, const std::string &fileName);

folderTree getFolderTree(const std::string &targetPath, bool8_t bToLower = false, bool8_t bFullPath = false);

void log(eLogLevel level, const char8_t *pMessage, ...);

std::string getErrorMessage(uint32_t errorCode);

bool8_t executeProcess(const std::string &commandline, bool8_t bWaitUntilDone = true);

namespaceEnd(gear)


