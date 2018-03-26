#pragma once

#include <Windows.h>
#include <stdint.h>
#include <assert.h>
#include <memory>
#include "RefCountPtr.h"

/// http://blog.csdn.net/slshelly/article/details/8720487
#ifdef _WINDLL
	/// Use Dynamic Link
	#ifdef _EXPORT_DLL_
		#define D3DExport __declspec(dllexport)
	#else
		#define D3DExport __declspec(dllimport)
	#endif
#else
	/// Use Static Lib
	#define D3DExport
#endif

#define SafeRelease(ptr)      { if((ptr) != nullptr) (ptr)->Release(); (ptr) = nullptr; }
#define SafeDelete(ptr)       { if((ptr) != nullptr) delete (ptr); (ptr) = nullptr; }
#define SafeDeleteArray(ptr)  { if((ptr) != nullptr) delete[] (ptr); (ptr) = nullptr; }

#define HRCheck(func)  if (FAILED(func)) { assert(0); }

#define NamespaceBegin(name) namespace name {
#define NamespaceEnd(name) }
