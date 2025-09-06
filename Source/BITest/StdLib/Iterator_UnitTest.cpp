#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/Iterator.h"

TEST(Iterator, ArrayCBegin) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	auto itrCBegin = BIEngine::CBegin(arr);
	auto itrCEnd = BIEngine::CEnd(arr);

	int i = 0;
	for (; itrCBegin != itrCEnd; ++itrCBegin, ++i) {
		EXPECT_EQ(*itrCBegin, i);
	}

	EXPECT_EQ(i, arraySize);
}

TEST(Iterator, ArrayBegin) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	auto itrEnd = BIEngine::End(arr);

	int i = arraySize;
	for (auto itrBegin = BIEngine::Begin(arr); itrBegin != itrEnd; ++itrBegin, --i) {
		*itrBegin = i;
	}

	i = arraySize;
	for (auto itrBegin = BIEngine::Begin(arr); itrBegin != itrEnd; ++itrBegin, --i) {
		EXPECT_EQ(*itrBegin, i);
	}
}

TEST(Iterator, ReverseIterator) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	auto itrBegin = BIEngine::Begin(arr);
	auto itrEnd = BIEngine::End(arr);

	BIEngine::ReverseIterator rBegin(itrEnd);
	BIEngine::ReverseIterator rEnd(itrBegin);

	int i = arraySize;
	for (; rBegin != rEnd; ++rBegin, --i) {
		EXPECT_EQ(*rBegin, i - 1);
	}

	EXPECT_EQ(i, 0);
}