#include <googletest/googletest/include/gtest/gtest.h>

#include "sort.h"
#include "FakeSTL/vector.hpp"

template <typename T> class gTest_Sorting
{
public:
	gTest_Sorting(uint32_t count, uint32_t range, std::function<bool8_t(const T &, const T &)> compFunc = [](const T &left, const T &right)->bool8_t {
		return left <= right;
	})
		: m_Count(count)
		, m_Range(range)
		, m_CompareFunc(compFunc)
	{
		assert(m_Count > 0u && m_Range > 0u);
	}

	bool8_t doTest()
	{
		uint32_t factor = (uint32_t)rand() % 10u;

		for (uint32_t i = 0u; i < m_Count; ++i)
		{
			T *data = new T[m_Range]();
			assert(data);
			for (uint32_t j = 0u; j < m_Range; ++j)
			{
				data[j] = (T)(rand() / (T)factor);
			}
			std::vector<T> vData(m_Range);
			verify(memcpy_s(vData.data(), sizeof(T) * m_Range, data, sizeof(T) * m_Range) == 0);

			std::sort(vData.begin(), vData.end());
			fort::insertSort<T>(data, data + m_Range, m_CompareFunc);

			if (memcmp(data, vData.data(), sizeof(T) * m_Range) != 0)
			{
				for (uint32_t n = 0u; n < m_Range; ++n)
				{
					std::cout << data[n] << " ";
				}
				std::cout << std::endl;

				for (uint32_t n = 0u; n < m_Range; ++n)
				{
					std::cout << vData[n] << " ";
				}
				std::cout << std::endl;

				safeDeleteArray(data);
				return false;
			}

			safeDeleteArray(data);
		}

		return true;
	}
protected:
private:
	uint32_t m_Count = 0u;
	uint32_t m_Range = 0u;
	std::function<bool8_t(const T &, const T &)> m_CompareFunc;
};

int32_t main(int32_t argc, char8_t **argv)
{	
	struct Test
	{
		uint32_t Value = 3u;
		~Test()
		{
			std::cout << "Test" << std::endl;
		}
	};

	std::vector<uint64_t> test(3, 7);
	auto maxSize = test.max_size();
	FakeSTL::Vector<uint64_t> testV(3, 7u);
	maxSize = testV.max_size();
	if (!testV.empty())
	{
		FakeSTL::Vector<Test> test2(3);

		FakeSTL::Vector<uint64_t> test3{1, 2, 3};

		auto v = test3[2];
		auto tv = test2[1];
		auto tvv = test2.back();
		auto vvt = test.back();
	}

	///::srand((uint32_t)(time(nullptr)));

	///testing::InitGoogleTest(&argc, argv);

	///return RUN_ALL_TESTS();

	return 0;
}

#pragma region sorting
TEST(Sorting, InsertSorting_UInt32)
{
	gTest_Sorting<uint32_t> tester(0xFFF, 0xFF);
	EXPECT_TRUE(tester.doTest() == true);
}

TEST(Sorting, InsertSorting_Int32)
{
	gTest_Sorting<int32_t> tester(0xFFF, 0xFF);
	EXPECT_TRUE(tester.doTest() == true);
}

TEST(Sorting, InsertSorting_Float)
{
	gTest_Sorting<float32_t> tester(0xFFF, 0xFF);
	EXPECT_TRUE(tester.doTest() == true);
}

TEST(Sorting, InsertSorting_Int8)
{
	gTest_Sorting<int8_t> tester(0xFFF, 0xFF);
	EXPECT_TRUE(tester.doTest() == true);
}

TEST(Sorting, InsertSorting_UInt8)
{
	gTest_Sorting<uint8_t> tester(0xFFF, 0xFF);
	EXPECT_TRUE(tester.doTest() == true);
}

TEST(Sorting, InsertSorting_Double)
{
	gTest_Sorting<double64_t> tester(0xFFF, 0xFF);
	EXPECT_TRUE(tester.doTest() == true);
}
#pragma endregion sorting