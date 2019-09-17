#include "linearlist.h"
#include "linkedlist.h"
#include "bintree.h"
#include "redblacktree.h"
#include "sort.h"
#include "recursion.h"

#include <array>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <forward_list>
#include <deque>

///#define TestLinearList
#define TestLinkedList

#include <map>

struct InputBuffer
{
	size_t Address;
	const void *State;
};

__declspec(align(16ull)) struct InputBuffer_0
{
	size_t Address;
	const void *State;
};

struct AlignTest
{
	uint32_t Done;

	InputBuffer Buffer;

	void *Next;
};

__declspec(align(16ull)) struct AlignTest_1
{
	uint32_t Done;
	void *Cast;
};

struct AlignTest_0
{
	__declspec(align(16ull)) uint32_t Done;

	InputBuffer Buffer;

	__declspec(align(16ull)) void *Next;
};

class Test
{
public:
private:
	void Out()
	{
	}
};

class CTest : private Test
{
public:
	void OutC()
	{
		////return Out();
	}
};

constexpr int Tset()
{
	return 1;
}

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
#endif

#if 0
	uint32_t numbers[20U] = {};
	for (uint32_t i = 0U; i < 20U; ++i)
	{
		numbers[i] = (std::rand() % 100U);
	}
	insertSort(&numbers[0U], &numbers[19U] + 1U);

	for (uint32_t i = 0U; i < 20U; ++i)
	{
		numbers[i] = (std::rand() % 100U);
	}

	int j = 12;
	auto b = [=] { return j + 1; };
	auto a = [&] { return j + 1; };
	std::cout << b() << std::endl;
	std::cout << a() << std::endl;

	++j;
	std::cout << b() << std::endl;
	std::cout << a() << std::endl;

	std::map<char, char> mapTest;
	mapTest.insert(std::make_pair('A', 'B'));
	mapTest.insert(std::make_pair('B', 'B'));
	mapTest.insert(std::make_pair('D', 'B'));
	mapTest.insert(std::make_pair('F', 'B'));
	mapTest.insert(std::make_pair('E', 'B'));
	mapTest.insert(std::make_pair('C', 'B'));

	for (auto it = mapTest.begin(); it != mapTest.end(); ++it)
	{
		std::cout << it->first << "=>" << it->second << std::endl;
	}
#endif

	//BinTree<char> TreeTest;
	//TreeTest.Insert('A');
	//TreeTest.Insert('B');
	//TreeTest.Insert('C');
	//TreeTest.Insert('D');
	//TreeTest.Insert('E');
	//TreeTest.Insert('F');
	//TreeTest.Insert('G');

	//TreeTest.TraverseByPreOrder();
	//TreeTest.TraverseByInOrder();
	//TreeTest.TraverseByPostOrder();

	//rbTree<char> rbTreeTest;

	InputBuffer buffer0;
	InputBuffer_0 buffer_0;

	AlignTest test0;
	AlignTest_0 test_0;

	AlignTest_1 test1;

	Hanoi(3u, 'A', 'B', 'C');

	OutputDebugStringA("\n\n");

	_Hanoi_(3u, 'A', 'B', 'C');
	
	return 0;
}