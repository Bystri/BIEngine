#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/Numeric.h"
#include "../../BIEngine/StdLib/Iterator.h"

TEST(Numeric, Accumulate) {
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	constexpr int InitialVal = 11;
	const int sum = BIEngine::Accumulate(BIEngine::Begin(arr), BIEngine::End(arr), InitialVal);

	EXPECT_EQ(sum, 21);
}

TEST(Numeric, AccumulateWithOp) {
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	constexpr int InitialVal = 11;
	const int sum = BIEngine::Accumulate(BIEngine::Begin(arr), BIEngine::End(arr), InitialVal, BIEngine::Minus<int>());

	EXPECT_EQ(sum, 1);
}

TEST(Numeric, InnerProduct) {
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}
	
	constexpr int InitialVal = 11;
	const int sum = BIEngine::InnerProduct(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arr), InitialVal);

	EXPECT_EQ(sum, 41);
}

TEST(Numeric, InnerProductWithOp) {
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	constexpr int InitialVal = 11;
	const int sum = BIEngine::InnerProduct(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arr), InitialVal, BIEngine::Minus<int>(), BIEngine::Plus<int>());

	EXPECT_EQ(sum, -9);
}

TEST(Numeric, PartialSum) {
	constexpr int arraySize = 5;

	int arr[arraySize];
	int arrToCopy[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
		arrToCopy[i] = 0;
	}

	const auto itr = BIEngine::PartialSum(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arrToCopy));

	EXPECT_EQ(itr, BIEngine::End(arrToCopy));

	EXPECT_EQ(arrToCopy[0], 0);
	EXPECT_EQ(arrToCopy[1], 1);
	EXPECT_EQ(arrToCopy[2], 3);
	EXPECT_EQ(arrToCopy[3], 6);
	EXPECT_EQ(arrToCopy[4], 10);
}

TEST(Numeric, PartialSumWithOp) {
	constexpr int arraySize = 5;

	int arr[arraySize];
	int arrToCopy[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
		arrToCopy[i] = 0;
	}

	const auto itr = BIEngine::PartialSum(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arrToCopy), BIEngine::Minus<int>());

	EXPECT_EQ(itr, BIEngine::End(arrToCopy));

	EXPECT_EQ(arrToCopy[0], 0);
	EXPECT_EQ(arrToCopy[1], -1);
	EXPECT_EQ(arrToCopy[2], -3);
	EXPECT_EQ(arrToCopy[3], -6);
	EXPECT_EQ(arrToCopy[4], -10);
}

TEST(Numeric, AdjacentDifference) {
	constexpr int arraySize = 5;

	int arr[arraySize];
	int arrToCopy[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
		arrToCopy[i] = 0;
	}

	const auto itr = BIEngine::AdjacentDifference(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arrToCopy));

	EXPECT_EQ(itr, BIEngine::End(arrToCopy));

	EXPECT_EQ(arrToCopy[0], 0);
	EXPECT_EQ(arrToCopy[1], 1);
	EXPECT_EQ(arrToCopy[2], 1);
	EXPECT_EQ(arrToCopy[3], 1);
	EXPECT_EQ(arrToCopy[4], 1);
}

TEST(Numeric, AdjacentDifferenceWithOp) {
	constexpr int arraySize = 5;

	int arr[arraySize];
	int arrToCopy[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
		arrToCopy[i] = 0;
	}

	const auto itr = BIEngine::AdjacentDifference(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arrToCopy), BIEngine::Plus<int>());

	EXPECT_EQ(itr, BIEngine::End(arrToCopy));

	EXPECT_EQ(arrToCopy[0], 0);
	EXPECT_EQ(arrToCopy[1], 1);
	EXPECT_EQ(arrToCopy[2], 3);
	EXPECT_EQ(arrToCopy[3], 5);
	EXPECT_EQ(arrToCopy[4], 7);
}