#include <googletest/googletest/include/gtest/gtest.h>

#include "sort.h"

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
				safeDeleteArray(data);
				__debugbreak();
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
	::srand((uint32_t)(time(nullptr)));

	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
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