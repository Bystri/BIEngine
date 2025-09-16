#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/Algorithm.h"
#include "../../BIEngine/StdLib/Iterator.h"


TEST(Algortihm, Count) {
	constexpr int valToCount = 0;
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	arr[2] = valToCount;
	arr[4] = valToCount;

	int cnt = BIEngine::Count(BIEngine::Begin(arr), BIEngine::End(arr), valToCount);

	EXPECT_EQ(cnt, 3);
}

TEST(Algortihm, CountIf) {
	constexpr int valToCount = 0;
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	arr[2] = valToCount;
	arr[4] = valToCount;

	int cnt = BIEngine::CountIf(BIEngine::Begin(arr), BIEngine::End(arr), [](int val) { return val == 0; });

	EXPECT_EQ(cnt, 3);
}

TEST(Algortihm, MinElement) {
	constexpr int minValueToInsertIntoArray = -1;
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	arr[2] = minValueToInsertIntoArray;

	auto minItr = BIEngine::MinElement(BIEngine::Begin(arr), BIEngine::End(arr));

	EXPECT_EQ(*minItr, minValueToInsertIntoArray);
}

TEST(Algortihm, MinElementWithOp) {
	constexpr int minValueToInsertIntoArray = -1;
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	arr[2] = minValueToInsertIntoArray;

	auto minItr = BIEngine::MinElement(BIEngine::Begin(arr), BIEngine::End(arr), [](int lhs, int rhs) {return lhs > rhs; });

	EXPECT_EQ(*minItr, 4);
}

TEST(Algortihm, MaxElement) {
	constexpr int maxValueToInsertIntoArray = 10;
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	arr[2] = maxValueToInsertIntoArray;

	auto maxItr = BIEngine::MaxElement(BIEngine::Begin(arr), BIEngine::End(arr));

	EXPECT_EQ(*maxItr, maxValueToInsertIntoArray);
}

TEST(Algortihm, MaxElementWithOp) {
	constexpr int maxValueToInsertIntoArray = 10;
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	arr[2] = maxValueToInsertIntoArray;

	auto maxItr = BIEngine::MaxElement(BIEngine::Begin(arr), BIEngine::End(arr), [](int lhs, int rhs) {return lhs < rhs; });

	EXPECT_EQ(*maxItr, 0);
}

TEST(Algortihm, MinMaxElement) {
	constexpr int minValueToInsertIntoArray = -1;
	constexpr int maxValueToInsertIntoArray = 10;
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	arr[2] = minValueToInsertIntoArray;
	arr[4] = maxValueToInsertIntoArray;

	auto minMaxPair = BIEngine::MinMaxElement(BIEngine::Begin(arr), BIEngine::End(arr));

	EXPECT_EQ(*minMaxPair.first, minValueToInsertIntoArray);
	EXPECT_EQ(*minMaxPair.second, maxValueToInsertIntoArray);
}

TEST(Algortihm, MinMaxElementWithOp) {
	constexpr int minValueToInsertIntoArray = -1;
	constexpr int maxValueToInsertIntoArray = 10;
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	arr[2] = minValueToInsertIntoArray;
	arr[4] = maxValueToInsertIntoArray;

	auto minMaxPair = BIEngine::MinMaxElement(BIEngine::Begin(arr), BIEngine::End(arr), [](int lhs, int rhs) {return lhs > rhs; });

	EXPECT_EQ(*minMaxPair.first, maxValueToInsertIntoArray);
	EXPECT_EQ(*minMaxPair.second, minValueToInsertIntoArray);
}

TEST(Algortihm, Find) {
	constexpr int valToFind = 3;
	constexpr int valToFindButNotInArray = 10;
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	auto foundItr = BIEngine::Find(BIEngine::Begin(arr), BIEngine::End(arr), valToFind);
	auto notFoundItr = BIEngine::Find(BIEngine::Begin(arr), BIEngine::End(arr), valToFindButNotInArray);

	EXPECT_EQ(*foundItr, valToFind);
	EXPECT_EQ(notFoundItr, BIEngine::End(arr));
}

TEST(Algortihm, FindIf) {
	constexpr int valToFind = 3;
	constexpr int valToFindButNotInArray = 10;
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	auto foundItr = BIEngine::FindIf(BIEngine::Begin(arr), BIEngine::End(arr), [valToFind = valToFind](int i) { return i == valToFind; });
	auto notFoundItr = BIEngine::FindIf(BIEngine::Begin(arr), BIEngine::End(arr), [valToFindButNotInArray = valToFindButNotInArray](int i) { return i == valToFindButNotInArray; });

	EXPECT_EQ(*foundItr, valToFind);
	EXPECT_EQ(notFoundItr, BIEngine::End(arr));
}

TEST(Algortihm, FindIfNot) {
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	auto foundItr = BIEngine::FindIfNot(BIEngine::Begin(arr), BIEngine::End(arr), [arraySize = arraySize](int i) { return i >= arraySize; });
	auto notFoundItr = BIEngine::FindIfNot(BIEngine::Begin(arr), BIEngine::End(arr), [arraySize = arraySize](int i) { return i < arraySize; });

	EXPECT_EQ(*foundItr, 0);
	EXPECT_EQ(notFoundItr, BIEngine::End(arr));
}

TEST(Algortihm, AllOf) {
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	bool expectedTrue = BIEngine::AllOf(BIEngine::Begin(arr), BIEngine::End(arr), [arraySize = arraySize](int i) { return i < arraySize; });
	bool expectedFalse = BIEngine::AllOf(BIEngine::Begin(arr), BIEngine::End(arr), [arraySize = arraySize](int i) { return i >= arraySize; });

	EXPECT_TRUE(expectedTrue);
	EXPECT_FALSE(expectedFalse);
}

TEST(Algortihm, AnyOf) {
	constexpr int valToFind = 3;
	constexpr int valToFindButNotInArray = 10;
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	bool expectedTrue = BIEngine::AnyOf(BIEngine::Begin(arr), BIEngine::End(arr), [valToFind = valToFind](int i) { return i == valToFind; });
	bool expectedFalse = BIEngine::AnyOf(BIEngine::Begin(arr), BIEngine::End(arr), [valToFindButNotInArray = valToFindButNotInArray](int i) { return i == valToFindButNotInArray; });

	EXPECT_TRUE(expectedTrue);
	EXPECT_FALSE(expectedFalse);
}

TEST(Algortihm, NoneOf) {
	constexpr int arraySize = 5;

	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	bool expectedTrue = BIEngine::NoneOf(BIEngine::Begin(arr), BIEngine::End(arr), [arraySize = arraySize](int i) { return i >= arraySize; });
	bool expectedFalse = BIEngine::NoneOf(BIEngine::Begin(arr), BIEngine::End(arr), [arraySize = arraySize](int i) { return i < arraySize; });

	EXPECT_TRUE(expectedTrue);
	EXPECT_FALSE(expectedFalse);
}


TEST(Algortihm, Copy) {
	constexpr int arraySize = 5;
	int arr[arraySize];
	int arrToCopy[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	BIEngine::Copy(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arrToCopy));

	for (int i = 0; i < arraySize; ++i) {
		EXPECT_EQ(arrToCopy[i], arr[i]);
	}
}


TEST(Algortihm, CopyIf) {
	constexpr int arraySize = 5;
	int arr[arraySize];
	int arrToCopy[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
		arrToCopy[i] = 0;
	}

	BIEngine::CopyIf(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arrToCopy), [](int val) { return val % 2 == 0; });

	EXPECT_EQ(arrToCopy[0], 0);
	EXPECT_EQ(arrToCopy[1], 2);
	EXPECT_EQ(arrToCopy[2], 4);
	EXPECT_EQ(arrToCopy[3], 0);
	EXPECT_EQ(arrToCopy[4], 0);
}


TEST(Algortihm, CopyN) {
	constexpr int arraySize = 5;
	constexpr int numToCopy = 3;
	int arr[arraySize];
	int arrToCopy[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
		arrToCopy[i] = 0;
	}

	BIEngine::CopyN(BIEngine::Begin(arr), 3, BIEngine::Begin(arrToCopy));

	for (int i = 0; i < arraySize; ++i) {
		if (i < numToCopy) {
			EXPECT_EQ(arrToCopy[i], i);
			continue;
		}
		EXPECT_EQ(arrToCopy[i], 0);
	}
}


TEST(Algortihm, Move) {
	constexpr int arraySize = 5;
	int arr[arraySize];
	int arrToCopy[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	BIEngine::Move(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arrToCopy));

	for (int i = 0; i < arraySize; ++i) {
		EXPECT_EQ(arrToCopy[i], i);
	}
}



TEST(Algortihm, Transform) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	BIEngine::Transform(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arr), [](int val) { return val *= 2; });

	for (int i = 0; i < arraySize; ++i) {
		EXPECT_EQ(arr[i], i * 2);
	}
}


TEST(Algortihm, Fill) {
	constexpr int arraySize = 5;
	constexpr int numToFill = 42;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	BIEngine::Fill(BIEngine::Begin(arr), BIEngine::End(arr), numToFill);

	for (int i = 0; i < arraySize; ++i) {
		EXPECT_EQ(arr[i], numToFill);
	}
}


TEST(Algortihm, FillN) {
	constexpr int arraySize = 5;
	constexpr int numToFill = 42;
	constexpr int fillNum = 3;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	BIEngine::FillN(BIEngine::Begin(arr), fillNum, numToFill);

	for (int i = 0; i < arraySize; ++i) {
		if (i < fillNum) {
			EXPECT_EQ(arr[i], numToFill);
			continue;
		}

		EXPECT_EQ(arr[i], i);
	}
}


TEST(Algortihm, Generate) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	constexpr int startValue = 43;
	int cnt = startValue;
	BIEngine::Generate(BIEngine::Begin(arr), BIEngine::End(arr), [&cnt]() {return cnt++; });

	for (int i = 0; i < arraySize; ++i) {
		EXPECT_EQ(arr[i], startValue + i);
	}
}


TEST(Algortihm, GenerateN) {
	constexpr int arraySize = 5;
	constexpr int generateNum = 3;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	constexpr int startValue = 43;
	int cnt = startValue;
	BIEngine::GenerateN(BIEngine::Begin(arr), generateNum, [&cnt]() {return cnt++; });

	for (int i = 0; i < arraySize; ++i) {
		if (i < generateNum) {
			EXPECT_EQ(arr[i], startValue + i);
			continue;
		}

		EXPECT_EQ(arr[i], i);
	}
}


TEST(Algortihm, Iota) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	constexpr int startValue = 43;
	BIEngine::Iota(BIEngine::Begin(arr), BIEngine::End(arr), startValue);

	for (int i = 0; i < arraySize; ++i) {
		EXPECT_EQ(arr[i], startValue + i);
	}
}

TEST(Algortihm, Replace) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	constexpr int oldVal = 3;
	constexpr int newVal = 43;
	BIEngine::Replace(BIEngine::Begin(arr), BIEngine::End(arr), oldVal, newVal);

	for (int i = 0; i < arraySize; ++i) {
		if (i == oldVal) {
			EXPECT_EQ(arr[i], newVal);
			continue;
		}
		EXPECT_EQ(arr[i], i);
	}
}


TEST(Algortihm, ReplaceIf) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	constexpr int oldVal = 3;
	constexpr int newVal = 43;
	BIEngine::ReplaceIf(BIEngine::Begin(arr), BIEngine::End(arr), [oldVal = oldVal](int val) {return val == oldVal; }, newVal);

	for (int i = 0; i < arraySize; ++i) {
		if (i == oldVal) {
			EXPECT_EQ(arr[i], newVal);
			continue;
		}
		EXPECT_EQ(arr[i], i);
	}
}

TEST(Algortihm, Remove) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	constexpr int valToDelete = 42;

	arr[0] = valToDelete;
	arr[2] = valToDelete;
	arr[4] = valToDelete;

	BIEngine::Remove(BIEngine::Begin(arr), BIEngine::End(arr), valToDelete);

	EXPECT_EQ(arr[0], 1);
	EXPECT_EQ(arr[1], 3);
}

TEST(Algortihm, RemoveIf) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	BIEngine::RemoveIf(BIEngine::Begin(arr), BIEngine::End(arr), [](int val) { return val % 2 != 0; });

	EXPECT_EQ(arr[0], 0);
	EXPECT_EQ(arr[1], 2);
	EXPECT_EQ(arr[2], 4);
}

TEST(Algortihm, RemoveCopy) {
	constexpr int arraySize = 5;
	int arr[arraySize];
	int arrCopyTo[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
		arrCopyTo[i] = 0;
	}

	constexpr int valToDelete = 42;

	arr[0] = valToDelete;
	arr[2] = valToDelete;
	arr[4] = valToDelete;

	BIEngine::RemoveCopy(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arrCopyTo), valToDelete);

	EXPECT_EQ(arrCopyTo[0], 1);
	EXPECT_EQ(arrCopyTo[1], 3);
	EXPECT_EQ(arrCopyTo[2], 0);
	EXPECT_EQ(arrCopyTo[3], 0);
	EXPECT_EQ(arrCopyTo[4], 0);
}

TEST(Algortihm, RemoveCopyIf) {
	constexpr int arraySize = 5;
	int arr[arraySize];
	int arrCopyTo[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
		arrCopyTo[i] = 0;
	}

	BIEngine::RemoveCopyIf(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arrCopyTo), [](int val) { return val % 2 != 0; });

	EXPECT_EQ(arrCopyTo[0], 0);
	EXPECT_EQ(arrCopyTo[1], 2);
	EXPECT_EQ(arrCopyTo[2], 4);
	EXPECT_EQ(arrCopyTo[3], 0);
	EXPECT_EQ(arrCopyTo[4], 0);
}

TEST(Algortihm, Unique) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	arr[1] = 3;
	arr[2] = 3;
	arr[3] = 3;

	BIEngine::Unique(BIEngine::Begin(arr), BIEngine::End(arr));

	EXPECT_EQ(arr[0], 0);
	EXPECT_EQ(arr[1], 3);
	EXPECT_EQ(arr[2], 4);
}

TEST(Algortihm, UniqueWithBinaryPred) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	arr[1] = 4;

	BIEngine::Unique(BIEngine::Begin(arr), BIEngine::End(arr), [](int a, int b) {return a % 2 == b % 2; });

	EXPECT_EQ(arr[0], 0);
	EXPECT_EQ(arr[1], 3);
	EXPECT_EQ(arr[2], 4);
}

TEST(Algortihm, UniqueCopy) {
	constexpr int arraySize = 5;
	int arr[arraySize];
	int arrCopyTo[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
		arrCopyTo[i] = 0;
	}

	arr[1] = 3;
	arr[2] = 3;
	arr[3] = 3;

	BIEngine::UniqueCopy(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arrCopyTo));

	EXPECT_EQ(arrCopyTo[0], 0);
	EXPECT_EQ(arrCopyTo[1], 3);
	EXPECT_EQ(arrCopyTo[2], 4);
	EXPECT_EQ(arrCopyTo[3], 0);
	EXPECT_EQ(arrCopyTo[4], 0);
}

TEST(Algortihm, UniqueCopyWithBinaryPred) {
	constexpr int arraySize = 5;
	int arr[arraySize];
	int arrCopyTo[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
		arrCopyTo[i] = 0;
	}

	arr[1] = 4;

	BIEngine::UniqueCopy(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arrCopyTo), [](int a, int b) {return a % 2 == b % 2; });

	EXPECT_EQ(arrCopyTo[0], 0);
	EXPECT_EQ(arrCopyTo[1], 3);
	EXPECT_EQ(arrCopyTo[2], 4);
	EXPECT_EQ(arrCopyTo[3], 0);
	EXPECT_EQ(arrCopyTo[4], 0);
}

TEST(Algortihm, Reverse) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	BIEngine::Reverse(BIEngine::Begin(arr), BIEngine::End(arr));

	for (int i = 0; i < arraySize; ++i) {
		EXPECT_EQ(arr[i], arraySize - i - 1);
	}
}

TEST(Algortihm, ReverseCopy) {
	constexpr int arraySize = 5;
	int arr[arraySize];
	int arrCopyTo[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
		arrCopyTo[i] = 0;
	}

	BIEngine::ReverseCopy(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Begin(arrCopyTo));

	for (int i = 0; i < arraySize; ++i) {
		EXPECT_EQ(arrCopyTo[i], arraySize - i - 1);
	}
}

TEST(Algortihm, Shuffle) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i;
	}

	std::random_device rd;
	std::mt19937 g(rd());
	BIEngine::Shuffle(BIEngine::Begin(arr), BIEngine::End(arr), g);

	bool hitTable[arraySize];
	for (int i = 0; i < arraySize; ++i) {
		hitTable[i] = false;
	}


	bool hasShuffledElem = false;
	for (int i = 0; i < arraySize; ++i) {
		hitTable[arr[i]] = true;

		if (arr[i] != i) {
			hasShuffledElem = true;
		}
	}

	EXPECT_TRUE(hasShuffledElem);

	for (int i = 0; i < arraySize; ++i) {
		EXPECT_TRUE(hitTable[arr[i]]);
	}
}

TEST(Algortihm, Sort) {
	constexpr int arraySize = 10;
	int arr[arraySize];

	arr[0] = 4;
	arr[1] = 3;
	arr[2] = 1;
	arr[3] = 2;
	arr[4] = 5;
	arr[5] = 9;
	arr[6] = 7;
	arr[7] = 10;
	arr[8] = 4;
	arr[9] = 6;

	BIEngine::Sort(BIEngine::Begin(arr), BIEngine::End(arr));

	EXPECT_EQ(arr[0], 1);
	EXPECT_EQ(arr[1], 2);
	EXPECT_EQ(arr[2], 3);
	EXPECT_EQ(arr[3], 4);
	EXPECT_EQ(arr[4], 4);
	EXPECT_EQ(arr[5], 5);
	EXPECT_EQ(arr[6], 6);
	EXPECT_EQ(arr[7], 7);
	EXPECT_EQ(arr[8], 9);
	EXPECT_EQ(arr[9], 10);
}

TEST(Algortihm, SortWithOp) {
	constexpr int arraySize = 10;
	int arr[arraySize];

	arr[0] = 4;
	arr[1] = 3;
	arr[2] = 1;
	arr[3] = 2;
	arr[4] = 5;
	arr[5] = 9;
	arr[6] = 7;
	arr[7] = 10;
	arr[8] = 4;
	arr[9] = 6;

	BIEngine::Sort(BIEngine::Begin(arr), BIEngine::End(arr), BIEngine::Greater<int>());

	EXPECT_EQ(arr[0], 10);
	EXPECT_EQ(arr[1], 9);
	EXPECT_EQ(arr[2], 7);
	EXPECT_EQ(arr[3], 6);
	EXPECT_EQ(arr[4], 5);
	EXPECT_EQ(arr[5], 4);
	EXPECT_EQ(arr[6], 4);
	EXPECT_EQ(arr[7], 3);
	EXPECT_EQ(arr[8], 2);
	EXPECT_EQ(arr[9], 1);
}

TEST(Algortihm, LowerBound) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i * 2;
	}

	constexpr int valToFind = 6;
	auto itrFound = BIEngine::LowerBound(BIEngine::Begin(arr), BIEngine::End(arr), valToFind);

	EXPECT_EQ(*itrFound, 6);

	constexpr int valNonExistInArray = 3;
	auto itrForNonExistInArray = BIEngine::LowerBound(BIEngine::Begin(arr), BIEngine::End(arr), valNonExistInArray);

	EXPECT_EQ(*itrForNonExistInArray, 4);
}

TEST(Algortihm, LowerBoundWithOp) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = arraySize - 1; i >= 0; --i) {
		arr[arraySize - i - 1] = i * 2;
	}

	constexpr int valToFind = 6;
	auto itrFound = BIEngine::LowerBound(BIEngine::Begin(arr), BIEngine::End(arr), valToFind, BIEngine::Greater<int>());

	EXPECT_EQ(*itrFound, 6);

	constexpr int valNonExistInArray = 3;
	auto itrForNonExistInArray = BIEngine::LowerBound(BIEngine::Begin(arr), BIEngine::End(arr), valNonExistInArray, BIEngine::Greater<int>());

	EXPECT_EQ(*itrForNonExistInArray, 2);
}

TEST(Algortihm, UpperBound) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i * 2;
	}

	constexpr int valToFind = 6;
	auto itrFound = BIEngine::UpperBound(BIEngine::Begin(arr), BIEngine::End(arr), valToFind);

	EXPECT_EQ(*itrFound, 8);

	constexpr int valNonExistInArray = 3;
	auto itrForNonExistInArray = BIEngine::UpperBound(BIEngine::Begin(arr), BIEngine::End(arr), valNonExistInArray);

	EXPECT_EQ(*itrForNonExistInArray, 4);
}

TEST(Algortihm, UpperBoundWithOp) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = arraySize - 1; i >= 0; --i) {
		arr[arraySize - i - 1] = i * 2;
	}

	constexpr int valToFind = 6;
	auto itrFound = BIEngine::UpperBound(BIEngine::Begin(arr), BIEngine::End(arr), valToFind, BIEngine::Greater<int>());

	EXPECT_EQ(*itrFound, 4);

	constexpr int valNonExistInArray = 3;
	auto itrForNonExistInArray = BIEngine::UpperBound(BIEngine::Begin(arr), BIEngine::End(arr), valNonExistInArray, BIEngine::Greater<int>());

	EXPECT_EQ(*itrForNonExistInArray, 2);
}

TEST(Algortihm, EqualRange) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i * 2;
	}

	constexpr int valToFind = 6;
	const auto itrFound = BIEngine::EqualRange(BIEngine::Begin(arr), BIEngine::End(arr), valToFind);

	EXPECT_EQ(*itrFound.first, valToFind);
	EXPECT_EQ(*itrFound.second, 8);

	constexpr int valNonExistInArray = 3;
	const auto itrForNonExistInArray = BIEngine::EqualRange(BIEngine::Begin(arr), BIEngine::End(arr), valNonExistInArray);

	EXPECT_EQ(*itrForNonExistInArray.first, 4);
	EXPECT_EQ(*itrForNonExistInArray.second, 4);
}

TEST(Algortihm, EqualRangeWithOp) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = arraySize - 1; i >= 0; --i) {
		arr[arraySize - i - 1] = i * 2;
	}

	constexpr int valToFind = 6;
	const auto itrFound = BIEngine::EqualRange(BIEngine::Begin(arr), BIEngine::End(arr), valToFind, BIEngine::Greater<int>());

	EXPECT_EQ(*itrFound.first, valToFind);
	EXPECT_EQ(*itrFound.second, 4);

	constexpr int valNonExistInArray = 3;
	const auto itrForNonExistInArray = BIEngine::EqualRange(BIEngine::Begin(arr), BIEngine::End(arr), valNonExistInArray, BIEngine::Greater<int>());

	EXPECT_EQ(*itrForNonExistInArray.first, 2);
	EXPECT_EQ(*itrForNonExistInArray.second, 2);
}

TEST(Algortihm, BinarySearch) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = 0; i < arraySize; ++i) {
		arr[i] = i * 2;
	}

	constexpr int valToFind = 6;
	const bool expectedTrue = BIEngine::BinarySearch(BIEngine::Begin(arr), BIEngine::End(arr), valToFind);

	EXPECT_TRUE(expectedTrue);

	constexpr int valNonExistInArray = 3;
	const bool expectedFalse = BIEngine::BinarySearch(BIEngine::Begin(arr), BIEngine::End(arr), valNonExistInArray);

	EXPECT_FALSE(expectedFalse);
}

TEST(Algortihm, BinarySearchWithOp) {
	constexpr int arraySize = 5;
	int arr[arraySize];

	for (int i = arraySize - 1; i >= 0; --i) {
		arr[arraySize - i - 1] = i * 2;
	}

	constexpr int valToFind = 6;
	const bool expectedTrue = BIEngine::BinarySearch(BIEngine::Begin(arr), BIEngine::End(arr), valToFind, BIEngine::Greater<int>());

	EXPECT_TRUE(expectedTrue);

	constexpr int valNonExistInArray = 3;
	const bool expectedFalse = BIEngine::BinarySearch(BIEngine::Begin(arr), BIEngine::End(arr), valNonExistInArray, BIEngine::Greater<int>());

	EXPECT_FALSE(expectedFalse);
}