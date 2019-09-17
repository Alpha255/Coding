#pragma once

#include "node.h"

/// O(N^2) steady
template <typename T> void insertSort(T *pBegin, T *pEnd, std::function<bool(T &, T &)> compFunc = [&](T &left, T &right)->bool {
	return left <= right;
})
{
	ptrdiff_t length = std::distance(pBegin, pEnd);
	uint32_t begin = 1U;
	int32_t i = 0U;

	while (begin < length)
	{
		T key = pBegin[begin];
		for (i = begin - 1; i >= 0; --i)
		{
			if (compFunc(pBegin[i], key))
			{
				pBegin[i + 1] = key;
				break;
			}
			pBegin[i + 1] = pBegin[i];
		}

		if (i < 0)
		{
			pBegin[0] = key;
		}

		++begin;
	}
}

/// O(n^1.3) not steady
template <typename T> void shellSort(T *pBegin, T *pEnd, std::function<bool(T &, T &)> compFunc = [&](T &left, T &right)->bool {
	return left <= right;
})
{
	ptrdiff_t length = std::distance(pBegin, pEnd);
	uint32_t incr = (uint32_t)length;

	while (incr > 1U)
	{
		incr = incr / 3U + 1U;
		for (uint32_t i = incr; i < length; ++i)
		{
			T key = pBegin[i];
			uint32_t start = i - incr;
			while (start >= 0U && key <= pBegin[start])
			{
				pBegin[start + incr] = pBegin[start];
				start -= incr;
			}

			pBegin[start + incr] = key;
		}
	}
}
