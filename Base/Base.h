#pragma once

#include <stdint.h>
#include <assert.h>
#include <memory>
#include <functional>
#include <array>
#include <vector>
#include <string>
#include <thread>
#include <fstream>
#include <DirectXMath.h>
#include <Windows.h>

///#ifdef _WINDLL
///	/// Use Dynamic Link
///	#ifdef _EXPORT_DLL_
///		#define Export __declspec(dllexport)
///	#else
///		#define Export __declspec(dllimport)
///	#endif
///#else
///	/// Use Static Lib
///	#define Export
///#endif

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

std::string GetFileExtension(const std::string &filePath, bool bToLower);

void BuildFileList(std::vector<std::string> &outFileList, const std::string &targetPath, const std::vector<std::string> filters, bool bToLower);

bool IsValidDirectory(const std::string &targetPath);

bool IsFileExists(const std::string &filePath);

void Log(const char *pFormatMsg, ...);

///std::string GetShaderCode(const char *pFileName, const char *pRenderTag, uint32_t shaderType);

NamespaceEnd(Base)

#include "BaseMath.h"
