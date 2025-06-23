#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/Array.h"

TEST(Array, ModifyElement) {
	BIEngine::Array<int, 1> arr;

	constexpr int testVal = 10;
	arr[0] = 10;
	EXPECT_EQ(arr[0], testVal);
}

TEST(Array, Data) {
	constexpr BIEngine::SizeT arraySize = 5;

	BIEngine::Array<int, arraySize> arr1;

	{
		int i = 0;
		auto data = arr1.Data();
		for (int i = 0; i < arr1.Size(); ++i) {
			data[i] = i;
		}
	}

	const BIEngine::Array<int, arraySize> arr2(arr1);

	{
		int i = 0;
		auto data = arr2.Data();
		for (int i = 0; i < arraySize; ++i) {
			EXPECT_EQ(data[i], i);
		}
	}

	{
		int i = 0;
		for (const auto& val : arr2) {
			EXPECT_EQ(val, i);
			++i;
		}
	}
}


TEST(Array, Front) {
	constexpr BIEngine::SizeT arraySize = 2;
	constexpr int testVal = 10;

	BIEngine::Array<int, arraySize> arr1;
	arr1[0] = testVal;

	EXPECT_EQ(arr1.Front(), testVal);

	const BIEngine::Array<int, arraySize> arr2(arr1);
	EXPECT_EQ(arr2.Front(), testVal);
}

TEST(Array, Back) {
	constexpr BIEngine::SizeT arraySize = 2;
	constexpr int testVal = 10;

	BIEngine::Array<int, arraySize> arr1;
	arr1[arraySize - 1] = testVal;

	EXPECT_EQ(arr1.Back(), testVal);

	const BIEngine::Array<int, arraySize> arr2(arr1);
	EXPECT_EQ(arr2.Back(), testVal);
}

TEST(Array, Iterators) {
	constexpr BIEngine::SizeT arraySize = 5;

	BIEngine::Array<int, arraySize> arr;

	int i = 0;
	for (auto itr = arr.Begin(); itr != arr.End(); ++itr, ++i) {
		*itr = i;
	}

	for (int i = 0; i < arraySize; ++i) {
		EXPECT_EQ(arr[i], i);
	}
}


TEST(Array, ConstIterators) {
	constexpr BIEngine::SizeT arraySize = 5;

	BIEngine::Array<int, arraySize> arr1;

	{
		int i = 0;
		for (auto itr = arr1.Begin(); itr != arr1.End(); ++itr, ++i) {
			*itr = i;
		}
	}

	const BIEngine::Array<int, arraySize> arr2(arr1);

	{
		int i = 0;
		for (auto itr = arr2.CBegin(); itr != arr2.CEnd(); ++itr, ++i) {
			EXPECT_EQ(*itr, i);
		}
	}
}

TEST(Array, Fill) {
	constexpr BIEngine::SizeT arraySize = 5;
	BIEngine::Array<int, 5> arr;

	constexpr int testVal = 10;
	arr.Fill(testVal);

	for (const auto& val : arr) {
		EXPECT_EQ(val, testVal);
	}
}