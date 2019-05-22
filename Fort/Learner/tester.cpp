#include "linearlist.h"
#include "linkedlist.h"

#define TestLinearList

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

	test.remove(6);

	linearlist<int32_t> testMove(std::move(test));
	linearlist<int32_t> testCopy(testMove);
#endif

#if defined(TestLinkedList)

#endif
	
	return 0;
}