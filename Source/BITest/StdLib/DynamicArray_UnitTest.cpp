#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/DynamicArray.h"

TEST(DynamicArray, PushBack) {
	constexpr int valsToAdd = 10;

	BIEngine::DynamicArray<int> dynArr;

	for (int i = 0; i < valsToAdd; ++i) {
		dynArr.PushBack(i);
	}

	for (int i = 0; i < valsToAdd; ++i) {
		EXPECT_EQ(dynArr[i], i);
	}
}

TEST(DynamicArray, EmplaceBack) {
	struct EmplaceBackTestObj
	{
		EmplaceBackTestObj(int _a, int _b)
			: a(_a)
			, b(_b)
		{

		}

		int a;
		int b;
	};

	BIEngine::DynamicArray<EmplaceBackTestObj> dynArr;
	dynArr.EmplaceBack(1, 2);

	EXPECT_EQ(dynArr[0].a, 1);
	EXPECT_EQ(dynArr[0].b, 2);
}

TEST(DynamicArray, Reserve) {
	constexpr int valsToAdd = 10;

	BIEngine::DynamicArray<int> dynArr;
	dynArr.Reserve(valsToAdd);

	EXPECT_EQ(dynArr.Capacity(), valsToAdd);

	for (int i = 0; i < valsToAdd; ++i) {
		dynArr.PushBack(i);
	}

	EXPECT_EQ(dynArr.Capacity(), valsToAdd);
}

TEST(DynamicArray, ModifyElement) {
	constexpr int valsToAdd = 10;
	constexpr int testVal = 123;

	BIEngine::DynamicArray<int> dynArr;

	for (int i = 0; i <= valsToAdd; ++i) {
		dynArr.PushBack(i);
	}

	dynArr[0] = testVal;

	EXPECT_EQ(dynArr[0], testVal);
}

TEST(DynamicArray, CapacityChanging) {
	constexpr int valsToAdd = 10;

	BIEngine::DynamicArray<int> dynArr;
	const BIEngine::SizeT initialCapacity = dynArr.Capacity();

	for (int i = 0; i < valsToAdd; ++i) {
		dynArr.PushBack(0);
	}

	EXPECT_TRUE(initialCapacity < dynArr.Size() < dynArr.Capacity());
}

static int testCnt = 0;

class TestObject
{
public:
	TestObject()
	{
		++testCnt;
	}

	TestObject(const TestObject& rhs)
	{
		++testCnt;
	}

	TestObject(TestObject&& rhs)
	{
		++testCnt;
	}

	~TestObject()
	{
		--testCnt;
	}
};

TEST(DynamicArray, DefaultConstruction) {
	{
		BIEngine::DynamicArray<TestObject> dynArr;
	}

	//Test only creation and destruction of default object
	EXPECT_TRUE(true);
}


TEST(DynamicArray, HoldedObjectsDestruction) {
	testCnt = 0;
	{
		constexpr int valsToAdd = 10;

		BIEngine::DynamicArray<TestObject> dynArr;
		const BIEngine::SizeT initialCapacity = dynArr.Capacity();

		for (int i = 0; i < valsToAdd; ++i) {
			TestObject obj;
			dynArr.PushBack(obj);
		}

		EXPECT_TRUE(testCnt == dynArr.Size());
	}

	EXPECT_TRUE(testCnt == 0);
}

TEST(DynamicArray, CopyCtor) {
	constexpr int valsNum = 10;

	BIEngine::DynamicArray<int> dynArr1;
	
	for (int i = 0; i < valsNum; ++i) {
		dynArr1.PushBack(i);
	}

	const BIEngine::DynamicArray<int> dynArr2(dynArr1);

	for (int i = 0; i < valsNum; ++i) {
		EXPECT_EQ(dynArr2[i], i);
	}
}

TEST(DynamicArray, CopyAssignment) {
	constexpr int valsNum = 10;

	BIEngine::DynamicArray<int> dynArr1;

	for (int i = 0; i < valsNum; ++i) {
		dynArr1.PushBack(i);
	}

	const BIEngine::DynamicArray<int> dynArr2 = dynArr1;

	for (int i = 0; i < valsNum; ++i) {
		EXPECT_EQ(dynArr2[i], i);
	}
}

TEST(DynamicArray, MoveCtor) {
	constexpr int valsNum = 10;

	BIEngine::DynamicArray<int> dynArr1;

	for (int i = 0; i < valsNum; ++i) {
		dynArr1.PushBack(i);
	}

	const BIEngine::DynamicArray<int> dynArr2(std::move(dynArr1));

	for (int i = 0; i < valsNum; ++i) {
		EXPECT_EQ(dynArr2[i], i);
	}
}

TEST(DynamicArray, MoveAssignment) {
	constexpr int valsNum = 10;

	BIEngine::DynamicArray<int> dynArr2;

	{
		BIEngine::DynamicArray<int> dynArr1;

		for (int i = 0; i < valsNum; ++i) {
			dynArr1.PushBack(i);
		}

		dynArr2 = std::move(dynArr1);
	}

	for (int i = 0; i < valsNum; ++i) {
		EXPECT_EQ(dynArr2[i], i);
	}
}

TEST(DynamicArray, NCtor) {
	constexpr int valsToAdd = 10;

	BIEngine::DynamicArray<TestObject> dynArr(valsToAdd);

	EXPECT_TRUE(valsToAdd == dynArr.Size());

	EXPECT_TRUE(testCnt == dynArr.Capacity() && testCnt == dynArr.Size());
}

TEST(DynamicArray, NCopyCtor) {
	constexpr int numVals = 10;
	constexpr int valToAdd = 12345;

	BIEngine::DynamicArray<int> dynArr(numVals, valToAdd);

	for (int i = 0; i < numVals; ++i) {
		EXPECT_EQ(dynArr[i], valToAdd);
	}

	EXPECT_TRUE(numVals == dynArr.Capacity() && numVals == dynArr.Size());
}


TEST(DynamicArray, InitializerList) {

	BIEngine::DynamicArray<int> dynArr{0,1,2,3,4};

	for (int i = 0; i < 5; ++i) {
		EXPECT_EQ(dynArr[i], i);
	}
}

TEST(DynamicArray, Iterators) {
	constexpr int valsNum = 10;

	BIEngine::DynamicArray<int> dynArr;

	for (int i = 0; i < valsNum; ++i) {
		dynArr.PushBack(i);
	}

	{
		int i = 0;
		for (BIEngine::DynamicArray<int>::Iterator itr = dynArr.Begin(); itr != dynArr.End(); ++itr, ++i) {
			EXPECT_EQ(*itr, i);
		}
	}

	{
		int i = valsNum;
		for (BIEngine::DynamicArray<int>::Iterator itr = dynArr.Begin(); itr != dynArr.End(); ++itr, --i) {
			*itr = i;
		}
	}

	{
		int i = valsNum;
		for (BIEngine::DynamicArray<int>::ConstIterator itr = dynArr.CBegin(); itr != dynArr.CEnd(); ++itr, --i) {
			EXPECT_EQ(*itr, i);
		}
	}
}

TEST(DynamicArray, PopBack) {
	constexpr int valsNum = 10;

	BIEngine::DynamicArray<int> dynArr;

	for (int i = 0; i < valsNum; ++i) {
		dynArr.PushBack(i);
	}

	dynArr.PopBack();

	EXPECT_EQ(dynArr.Size(), valsNum - 1);

	{
		int i = 0;
		for (auto itr = dynArr.CBegin(); itr != dynArr.CEnd(); ++itr, ++i) {
			EXPECT_EQ(*itr, i);
		}
	}
}

TEST(DynamicArray, Insert) {
	constexpr int valsNum = 10;


	int destCount = 0;
	class InsertTestObj
	{
	public:
		InsertTestObj(int val, int* pCnt) : x(val), cnt(pCnt) {}

		InsertTestObj(const InsertTestObj& rhs) = default;
		InsertTestObj(InsertTestObj&& rhs) : x(rhs.x), cnt(rhs.cnt){rhs.cnt = nullptr;}

		InsertTestObj& operator=(const InsertTestObj& rhs) = default;

		~InsertTestObj()
		{
			if (cnt == nullptr) {
				return;
			}

			(*cnt)++;
		}
	
	public:
		int x;

	private:
		int* cnt = nullptr;
	};

	BIEngine::DynamicArray<InsertTestObj> dynArr;

	for (int i = 0; i < valsNum; ++i) {
		dynArr.PushBack(InsertTestObj(i, &destCount));
	}

	constexpr BIEngine::SizeT testIdx = 5;
	constexpr int testVal = 12345;

	dynArr.Insert(dynArr.Begin() + testIdx, InsertTestObj(testVal, &destCount));

	EXPECT_EQ(destCount, 0);
	EXPECT_EQ(dynArr.Size(), valsNum + 1);

	{
		int i = 0;
		for (auto itr = dynArr.CBegin(); itr != dynArr.CEnd(); ++itr, ++i) {
			if (itr - dynArr.CBegin() == testIdx) {
				EXPECT_EQ(itr->x, testVal);
				--i;
				continue;
			}

			EXPECT_EQ(itr->x, i);
		}
	}
}

TEST(DynamicArray, Erase) {
	constexpr int valsNum = 10;

	BIEngine::DynamicArray<int> dynArr;

	for (int i = 0; i < valsNum; ++i) {
		dynArr.PushBack(i);
	}

	constexpr BIEngine::SizeT testIdx = 5;

	dynArr.Erase(dynArr.Begin() + testIdx);

	{
		int i = 0;
		for (auto itr = dynArr.CBegin(); itr != dynArr.CEnd(); ++itr, ++i) {
			if (itr - dynArr.CBegin() == testIdx) {
				++i;
			}

			EXPECT_EQ(*itr, i);
		}
	}
}

TEST(DynamicArray, ResizeGrow) {
	constexpr int valsNum = 10;
	constexpr int valsAfterResizeNum = 20;

	BIEngine::DynamicArray<int> dynArr;

	for (int i = 0; i < valsNum; ++i) {
		dynArr.PushBack(i);
	}

	dynArr.Resize(valsAfterResizeNum);

	EXPECT_EQ(dynArr.Size(), valsAfterResizeNum);

	{
		int i = 0;
		for (auto itr = dynArr.CBegin(); itr != dynArr.CEnd(); ++itr, ++i) {
			if (itr - dynArr.CBegin() >= valsNum) {
				EXPECT_EQ(*itr, int());
				continue;
			}

			EXPECT_EQ(*itr, i);
		}
	}
}

TEST(DynamicArray, ResizeShrink) {
	constexpr int valsNum = 10;
	constexpr int valsAfterResizeNum = 5;

	BIEngine::DynamicArray<int> dynArr;

	for (int i = 0; i < valsNum; ++i) {
		dynArr.PushBack(i);
	}

	dynArr.Resize(valsAfterResizeNum);

	EXPECT_EQ(dynArr.Size(), valsAfterResizeNum);

	{
		int i = 0;
		for (auto itr = dynArr.CBegin(); itr != dynArr.CEnd(); ++itr, ++i) {
			EXPECT_EQ(*itr, i);
		}
	}
}