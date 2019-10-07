#pragma once

#include "Common.h"

namespaceStart(gear)

class noneCopyable
{
public:
	noneCopyable() = default;
	~noneCopyable() = default;

	noneCopyable(const noneCopyable &) = delete;
	void operator=(const noneCopyable &) = delete;
protected:
private:
};

template <typename T> class singleton : public noneCopyable
{
public:
	inline static T &instance()
	{
		return s_Instance;
	}

	singleton(const singleton &) = delete;
	void operator=(const singleton &) = delete;
protected:
	singleton() = default;
	virtual ~singleton() = default;
private:
	static T s_Instance;
};
template<class T> __declspec(selectany) T singleton<T>::s_Instance;

#define singletonDeclare(ClassName)          \
private:                                     \
	ClassName() = default;                   \
	~ClassName() = default;                  \
	friend class gear::singleton<ClassName>;

template <typename Left, typename Right>
inline bool isEqual(const Left &left, const Right &right)
{
	assert(sizeof(Left) == sizeof(Right));
	return ::memcmp(&left, &right, sizeof(Right)) == 0;
}

void toLower(std::string &srcStr);

void replace(std::string &srcStr, const char8_t srcC, char8_t dstC);

std::vector<std::string> split(const std::string &srcStr, char8_t token);

std::vector<std::string> split(const std::string &srcStr, const std::string &token);

std::string format(const char8_t *pArgStr, ...);

void log(const char8_t *pMessage, ...);

bool8_t executeProcess(const std::string &commandline, bool8_t bWaitUntilDone = true);

namespaceEnd(gear)


