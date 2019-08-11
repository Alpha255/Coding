#include "linearlist.h"
#include "linkedlist.h"

#include <list>

///#define TestLinearList
#define TestLinkedList

int main()
{
#if defined(TestLinearList)
	linearlist<int32_t> test(10U);
	for (size_t i = 0U; i < test.maxSize() / 2; ++i)
	{
		test.insert(i, (int32_t)i);
	}

	test.insert(2, -20);
	test.insert(4, -30);

	test.set(0, -40);

	test.erase(6);

	linearlist<int32_t> testMove(std::move(test));
	linearlist<int32_t> testCopy(testMove);
#endif

#if defined(TestLinkedList)
	linkedlist<int32_t> test;
	for (uint32_t i = 0U; i < 10; ++i)
	{
		test.push_back(i);
	}

	test.remove(7U);
	test.insert(7U, 155);

	test.clear();
#endif
	
	return 0;
}