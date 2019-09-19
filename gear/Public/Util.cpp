#include "Util.h"

namespaceStart(gear)

void toLower(std::string &srcStr)
{
	std::transform(srcStr.begin(), srcStr.end(), srcStr.begin(),
		[](char c)
	{
		return (char)::tolower((int32_t)c);
	});
}

void replace(std::string &srcStr, const char8_t srcC, const char8_t dstC)
{
	std::replace(srcStr.begin(), srcStr.end(), srcC, dstC);
}

template <typename Token> std::vector<std::string> split(const std::string &srcStr, const Token &token)
{
	std::vector<std::string> result;

	std::string temp(srcStr);
	size_t index = temp.find(token);

	while (index != std::string::npos)
	{
		std::string split = temp.substr(0u, index);
		if (split.length() > 0u)
		{
			result.emplace_back(split);
		}
		temp = temp.substr(index + 1U);
		index = temp.find(token);
	}

	if (temp.length() > 0U)
	{
		result.emplace_back(temp);
	}

	return result;
}

std::vector<std::string> split(const std::string &srcStr, const char8_t token)
{
	return split<char8_t>(srcStr, token);
}

std::vector<std::string> split(const std::string &srcStr, const std::string &token)
{
	return split<std::string>(srcStr, token);
}

std::string format(const char *pArgStr, ...)
{
	std::unique_ptr<char8_t> pResult = nullptr;
	if (pArgStr)
	{
		va_list list = nullptr;
		va_start(list, pArgStr);
		size_t size = (size_t)_vscprintf(pArgStr, list) + 1;
		pResult = std::unique_ptr<char8_t>(new char8_t[size]());
		_vsnprintf_s(pResult.get(), size, size, pArgStr, list);
		va_end(list);
	}

	return std::string(pResult.get());
}

void log(const char *pMessage, ...) 
{
#define formatString(message, ...) format(message, __VA_ARGS__)
	std::string message = formatString(pMessage);
	::OutputDebugStringA(message.c_str());
	::OutputDebugStringA("\n");
#undef formatString
}

namespaceEnd(gear)