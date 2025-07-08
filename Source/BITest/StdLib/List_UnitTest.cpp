#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/List.h"

TEST(List, PushBack) {
	constexpr int valsToAdd = 10;

	BIEngine::List<int> list;

	for (int i = 0; i <= valsToAdd; ++i) {
		list.PushBack(i);
	}

	int i = 0;
	for (auto itr = list.Begin(); itr != list.End(); ++itr, ++i) {
		EXPECT_EQ(*itr, i);
	}
	EXPECT_EQ(i, valsToAdd + 1); // Plus one bc we iterated to past-of-end in the loop
}

TEST(List, BackwardIteration) {
	constexpr int valsToAdd = 10;

	BIEngine::List<int> list;

	for (int i = 0; i <= valsToAdd; ++i) {
		list.PushBack(i);
	}

	auto itr = list.End();
	--itr;
	for (int i = valsToAdd; i >= 0; --itr, --i) {
		EXPECT_EQ(*itr, i);
	}
	EXPECT_EQ(itr, list.End());
}

TEST(List, PopBack) {
	constexpr int valsToAdd = 10;

	BIEngine::List<int> list;

	for (int i = 0; i < valsToAdd; ++i) {
		list.PushBack(i);
	}

	EXPECT_EQ(list.Size(), valsToAdd);

	list.PopBack();

	EXPECT_EQ(list.Size(), valsToAdd - 1);

	{
		int i = 0;
		for (auto itr = list.Begin(); itr != list.End(); ++itr, ++i) {
			EXPECT_EQ(*itr, i);
		}
		EXPECT_EQ(i, valsToAdd - 1);
	}

	for (int i = 0; i < valsToAdd - 1; ++i) {
		list.PopBack();
	}

	EXPECT_EQ(list.Size(), 0);
	EXPECT_EQ(list.Begin(), list.End());
}

TEST(List, PushFront) {
	constexpr int valsToAdd = 10;

	BIEngine::List<int> list;

	for (int i = 0; i <= valsToAdd; ++i) {
		list.PushFront(i);
	}

	int i = valsToAdd;
	for (auto itr = list.Begin(); itr != list.End(); ++itr, --i) {
		EXPECT_EQ(*itr, i);
	}
	EXPECT_EQ(i, -1);
}

TEST(List, PopFront) {
	constexpr int valsToAdd = 10;

	BIEngine::List<int> list;

	for (int i = 0; i < valsToAdd; ++i) {
		list.PushFront(i);
	}

	EXPECT_EQ(list.Size(), valsToAdd);

	list.PopFront();

	EXPECT_EQ(list.Size(), valsToAdd - 1);

	{
		int i = valsToAdd - 2;
		for (auto itr = list.Begin(); itr != list.End(); ++itr, --i) {
			EXPECT_EQ(*itr, i);
		}
		EXPECT_EQ(i, -1);
	}

	for (int i = 0; i < valsToAdd - 1; ++i) {
		list.PopFront();
	}

	EXPECT_EQ(list.Size(), 0);
	EXPECT_EQ(list.Begin(), list.End());
}

TEST(List, CopyCtor) {
	constexpr int valsToAdd = 10;

	BIEngine::List<int> list;

	for (int i = 0; i <= valsToAdd; ++i) {
		list.PushFront(i);
	}

	const BIEngine::List<int> listCopy(list);

	int i = valsToAdd;
	for (auto itr = listCopy.CBegin(); itr != listCopy.CEnd(); ++itr, --i) {
		EXPECT_EQ(*itr, i);
	}
	EXPECT_EQ(i, -1);
}

TEST(List, MoveCtor) {
	constexpr int valsToAdd = 10;

	BIEngine::List<int> list;

	for (int i = 0; i <= valsToAdd; ++i) {
		list.PushFront(i);
	}

	const BIEngine::List<int> listMoved(std::move(list));

	int i = valsToAdd;
	for (auto itr = listMoved.CBegin(); itr != listMoved.CEnd(); ++itr, --i) {
		EXPECT_EQ(*itr, i);
	}
	EXPECT_EQ(i, -1);
}

TEST(List, CopyAssignment) {
	constexpr int valsToAdd = 10;

	BIEngine::List<int> list;

	for (int i = 0; i <= valsToAdd; ++i) {
		list.PushFront(i);
	}

	BIEngine::List<int> listCopy;

	for (int i = 0; i <= valsToAdd; ++i) {
		listCopy.PushFront(12345);
	}

	listCopy = list;

	int i = valsToAdd;
	for (auto itr = listCopy.CBegin(); itr != listCopy.CEnd(); ++itr, --i) {
		EXPECT_EQ(*itr, i);
	}
	EXPECT_EQ(i, -1);
}

TEST(List, MoveAssignment) {
	constexpr int valsToAdd = 10;

	BIEngine::List<int> listMoved;

	for (int i = 0; i <= valsToAdd; ++i) {
		listMoved.PushFront(12345);
	}

	{
		BIEngine::List<int> list;

		for (int i = 0; i <= valsToAdd; ++i) {
			list.PushFront(i);
		}

		listMoved = std::move(list);
	}

	int i = valsToAdd;
	for (auto itr = listMoved.CBegin(); itr != listMoved.CEnd(); ++itr, --i) {
		EXPECT_EQ(*itr, i);
	}
	EXPECT_EQ(i, -1);
}

static int listTestObjCnt = 0;

class ListTestObj
{
public:
	ListTestObj()
	{
		++listTestObjCnt;
	}

	ListTestObj(int v)
		: val(v)
	{
		++listTestObjCnt;
	}

	~ListTestObj()
	{
		--listTestObjCnt;
	}

	int val = 0;
};

TEST(List, EmplaceBack) {
	constexpr int valsToAdd = 10;

	BIEngine::List<ListTestObj> list;

	for (int i = 0; i < valsToAdd; ++i) {
		list.EmplaceBack(i);
	}

	EXPECT_EQ(listTestObjCnt, valsToAdd);

	int i = 0;
	for (auto itr = list.CBegin(); itr != list.CEnd(); ++itr, ++i) {
		EXPECT_EQ(itr->val, i);
	}
	EXPECT_EQ(i, valsToAdd);
}

TEST(List, EmplaceFront) {
	constexpr int valsToAdd = 10;

	BIEngine::List<ListTestObj> list;

	for (int i = 0; i < valsToAdd; ++i) {
		list.EmplaceFront(i);
	}

	EXPECT_EQ(listTestObjCnt, valsToAdd);

	int i = valsToAdd - 1;
	for (auto itr = list.CBegin(); itr != list.CEnd(); ++itr, --i) {
		EXPECT_EQ(itr->val, i);
	}
	EXPECT_EQ(i, -1);
}

TEST(List, NCtor) {
	constexpr int valsToAdd = 10;

	BIEngine::List<ListTestObj> list(valsToAdd);

	EXPECT_EQ(list.Size(), valsToAdd);
	EXPECT_EQ(listTestObjCnt, valsToAdd);

	int i = 0;
	for (auto itr = list.CBegin(); itr != list.CEnd(); ++itr, ++i) {
		EXPECT_EQ(itr->val, 0);
	}
	EXPECT_EQ(i, valsToAdd);
}

TEST(List, Insert) {
	constexpr int valsToAdd = 10;
	constexpr int posToInsertBefore = 5;
	constexpr int valToInsert = 12345;

	BIEngine::List<int> list(valsToAdd);

	EXPECT_EQ(list.Size(), valsToAdd);

	{
		int i = 0;
		for (auto itr = list.Begin(); itr != list.End(); ++itr, ++i) {
			if (posToInsertBefore == i) {
				list.Insert(itr, valToInsert);
			}
		}
	}

	EXPECT_EQ(list.Size(), valsToAdd + 1);

	{
		int i = 0;
		for (auto itr = list.CBegin(); itr != list.CEnd(); ++itr, ++i) {
			if (posToInsertBefore == i) {
				EXPECT_EQ(*itr, valToInsert);
			} else {
				EXPECT_EQ(*itr, 0);
			}
		}
		EXPECT_EQ(i, valsToAdd + 1);
	}

	{
		auto itr = list.CEnd();
		--itr;
		for (int i = 0; i < valsToAdd + 1; --itr, ++i) {
			if (posToInsertBefore == i) {
				EXPECT_EQ(*itr, valToInsert);
			}
			else {
				EXPECT_EQ(*itr, 0);
			}
		}
	}
}

TEST(List, Erase) {
	constexpr int valsToAdd = 10;
	constexpr int posToErase = 5;

	BIEngine::List<int> list;

	for (int i = 0; i < valsToAdd; ++i) {
		list.PushBack(i);
	}

	{
		int i = 0;
		for (auto itr = list.Begin(); itr != list.End(); ++itr, ++i) {
			EXPECT_EQ(*itr, i);
			if (posToErase == i) {
				itr = list.Erase(itr);
				--itr;
			}
		}
		EXPECT_EQ(i, valsToAdd);
	}

	EXPECT_EQ(list.Size(), valsToAdd - 1);

	{
		int i = 0;
		for (auto itr = list.Begin(); itr != list.End(); ++itr, ++i) {
			if (posToErase == i) {
				++i;
			}

			EXPECT_EQ(*itr, i);
		}
		EXPECT_EQ(i, valsToAdd);
	}
}

TEST(List, Remove) {
	BIEngine::List<int> list;

	for (int i = 0; i < 10; ++i) {
		list.PushFront(i);
		list.PushBack(i);
	}

	list.Remove(4);

	EXPECT_EQ(list.Size(), 18);

	auto itrL = list.CBegin();
	auto itrR = list.CEnd();
	--itrR;
	for (int i = 9; i >= 0; --i) {
		if (i == 4) {
			EXPECT_NE(*itrL, i);
			EXPECT_NE(*itrR, i);
			continue;
		}

		EXPECT_EQ(*itrL, i);
		EXPECT_EQ(*itrR, i);
		++itrL;
		--itrR;
	}
}

TEST(List, ResizeGrow) {
	constexpr int valsToAdd = 10;
	constexpr int numToResize = 20;

	BIEngine::List<int> list;

	for (int i = 0; i < valsToAdd; ++i) {
		list.PushBack(i);
	}

	list.Resize(numToResize);

	EXPECT_EQ(list.Size(), numToResize);

	{
		int i = 0;
		for (auto itr = list.Begin(); itr != list.End(); ++itr, ++i) {
			if (i >= valsToAdd) {
				EXPECT_EQ(*itr, 0);
				continue;
			}
			
			EXPECT_EQ(*itr, i);
		}
		EXPECT_EQ(i, numToResize);
	}
}

TEST(List, ResizeShrink) {
	constexpr int valsToAdd = 10;
	constexpr int numToResize = 5;

	BIEngine::List<int> list;

	for (int i = 0; i < valsToAdd; ++i) {
		list.PushBack(i);
	}

	list.Resize(numToResize);

	EXPECT_EQ(list.Size(), numToResize);

	{
		int i = 0;
		for (auto itr = list.Begin(); itr != list.End(); ++itr, ++i) {
			EXPECT_EQ(*itr, i);
		}
		EXPECT_EQ(i, numToResize);
	}
}