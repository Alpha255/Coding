#pragma once

#include <stdint.h>
#include <assert.h>
#include <memory>
#include <functional>
#include <array>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <unordered_map>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <Windows.h>

///#define Export __declspec(dllexport)
/// #define Export __declspec(dllimport)
#define Export

#define SafeRelease(ptr)      { if((ptr) != nullptr) (ptr)->Release(); (ptr) = nullptr; }
#define SafeDelete(ptr)       { if((ptr) != nullptr) delete (ptr); (ptr) = nullptr; }
#define SafeDeleteArray(ptr)  { if((ptr) != nullptr) delete[] (ptr); (ptr) = nullptr; }

#define NamespaceBegin(name) namespace name {
#define NamespaceEnd(name) }

#define Verify(condition) if (!condition) { assert(0); }

NamespaceBegin(Base)

class NoneCopyable
{
public:
	NoneCopyable() = default;
	~NoneCopyable() = default;
	NoneCopyable(const NoneCopyable &) = delete;
	void operator=(const NoneCopyable &) = delete;
protected:
private:
};

template <typename T> struct Tree
{
	T Name;
	std::vector<std::shared_ptr<Tree>> Children;
};

template <typename Left, typename Right>
inline bool IsEqual(const Left &left, const Right &right)
{
	assert(sizeof(Left) == sizeof(Right));
	return memcmp(&left, &right, sizeof(Right)) == 0;
}

void ToLower(std::string &str);

void Replace(std::string &str, const char src, const char dst);

void Split(const std::string &src, const char token, std::vector<std::string> &result);

std::string FormatString(const char *pStr, ...);

std::string GetFileName(const std::string &src);

std::string GetFileExtension(const std::string &filePath, bool bToLower);

std::string GetRootDirectory(const std::string &path);

std::string GetParentDirectory(const std::string &path);

void BuildFileList(std::vector<std::string> &outFileList, const std::string &targetPath, const std::vector<std::string> &filters, bool bToLower);

void BuildFolderTree(Tree<std::string> &outTree, const std::string &targetPath, bool bToLower, bool bFullPath = false);

bool IsValidDirectory(const std::string &targetPath);

bool IsFileExists(const std::string &filePath);

::FILETIME GetFileLastWriteTime(const std::string &filePath);

void Log(const char *pMessage, ...);

NamespaceEnd(Base)

#include "BaseMath.h"
