#pragma once

#include "Gear/Definitions.h"

NAMESPACE_START(Gear)

class String
{
public:
	static void toLower(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(),
			[](char8_t c)
		{
			return static_cast<char8_t>(::tolower(static_cast<int32_t>(c)));
		});
	}

	static void replace(std::string& str, const std::string& src, const std::string& dst)
	{
		size_t i = 0u;
		while (i < str.length())
		{
			if (_strnicmp(str.c_str() + i, src.c_str(), src.length()) == 0)
			{
				str.replace(i, src.length(), dst);
				i += dst.length();
			}
			else
			{
				++i;
			}
		}
	}

	static std::vector<std::string> split(const std::string& str, const std::string& token)
	{
		std::vector<std::string> result;

		std::string temp(str);

		size_t index = temp.find(token);

		while (index != std::string::npos)
		{
			std::string split = temp.substr(0u, index);
			if (split.length() > 0u)
			{
				result.emplace_back(split);
			}
			temp = temp.substr(index + 1u);
			index = temp.find(token);
		}

		if (temp.length() > 0u)
		{
			result.push_back(temp);
		}

		return result;
	}

	static std::string format(const char8_t* fmt, ...)
	{
		if (fmt)
		{
			va_list list = nullptr;
			va_start(list, fmt);
			size_t size = (size_t)_vscprintf(fmt, list) + 1u;
			std::unique_ptr<char8_t[]> result = std::make_unique<char8_t[]>(size);
			_vsnprintf_s(result.get(), size, size, fmt, list);
			va_end(list);
			return std::string(result.get());
		}

		return std::string();
	}
};

NAMESPACE_END(Gear)


