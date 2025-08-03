#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/ForwardList.h"

TEST(ForwardList, PushFront) {
	constexpr int valsToAdd = 10;

	BIEngine::ForwardList<int> list;

	EXPECT_TRUE(list.Empty());

	for (int i = 0; i <= valsToAdd; ++i) {
		list.PushFront(i);
	}

	EXPECT_FALSE(list.Empty());

	int i = 10;
	for (auto itr = list.Begin(); itr != list.End(); ++itr, --i) {
		EXPECT_EQ(*itr, i);
	}
}

TEST(ForwardList, PopFront) {
	constexpr int valsToAdd = 10;

	BIEngine::ForwardList<int> list;

	for (int i = 0; i < valsToAdd; ++i) {
		list.PushFront(i);
	}

	list.PopFront();

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

	EXPECT_TRUE(list.Empty());
}


TEST(ForwardList, CopyCtor) {
	constexpr int valsToAdd = 10;

	BIEngine::ForwardList<int> list;

	for (int i = 0; i <= valsToAdd; ++i) {
		list.PushFront(i);
	}

	const BIEngine::ForwardList<int> listCopy(list);

	int i = valsToAdd;
	for (auto itr = listCopy.CBegin(); itr != listCopy.CEnd(); ++itr, --i) {
		EXPECT_EQ(*itr, i);
	}
	EXPECT_EQ(i, -1);
}

TEST(ForwardList, MoveCtor) {
	constexpr int valsToAdd = 10;

	BIEngine::ForwardList<int> list;

	for (int i = 0; i <= valsToAdd; ++i) {
		list.PushFront(i);
	}

	const BIEngine::ForwardList<int> listMoved(std::move(list));

	int i = valsToAdd;
	for (auto itr = listMoved.CBegin(); itr != listMoved.CEnd(); ++itr, --i) {
		EXPECT_EQ(*itr, i);
	}
	EXPECT_EQ(i, -1);
}


TEST(ForwardList, CopyAssignment) {
	constexpr int valsToAdd = 10;

	BIEngine::ForwardList<int> list;

	for (int i = 0; i <= valsToAdd; ++i) {
		list.PushFront(i);
	}

	BIEngine::ForwardList<int> listCopy;

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


TEST(ForwardList, MoveAssignment) {
	constexpr int valsToAdd = 10;

	BIEngine::ForwardList<int> listMoved;

	for (int i = 0; i <= valsToAdd; ++i) {
		listMoved.PushFront(12345);
	}

	{
		BIEngine::ForwardList<int> list;

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


static int forwardListTestObjCnt = 0;

class ForwardListTestObj
{
public:
	ForwardListTestObj()
	{
		++forwardListTestObjCnt;
	}

	ForwardListTestObj(int v)
		: val(v)
	{
		++forwardListTestObjCnt;
	}

	~ForwardListTestObj()
	{
		--forwardListTestObjCnt;
	}

	int val = 0;
};

TEST(ForwardList, EmplaceFront) {
	constexpr int valsToAdd = 10;

	BIEngine::ForwardList<ForwardListTestObj> list;

	for (int i = 0; i < valsToAdd; ++i) {
		list.EmplaceFront(i);
	}

	EXPECT_EQ(forwardListTestObjCnt, valsToAdd);

	int i = valsToAdd - 1;
	for (auto itr = list.CBegin(); itr != list.CEnd(); ++itr, --i) {
		EXPECT_EQ(itr->val, i);
	}
	EXPECT_EQ(i, -1);
}

TEST(ForwardList, NCtor) {
	constexpr int valsToAdd = 10;

	BIEngine::ForwardList<ForwardListTestObj> list(valsToAdd);

	EXPECT_FALSE(list.Empty(), valsToAdd);
	EXPECT_EQ(forwardListTestObjCnt, valsToAdd);

	int i = 0;
	for (auto itr = list.CBegin(); itr != list.CEnd(); ++itr, ++i) {
		EXPECT_EQ(itr->val, 0);
	}
	EXPECT_EQ(i, valsToAdd);
}

TEST(ForwardList, EmplaceAfter) {
	constexpr int valsToAdd = 10;
	constexpr int posToInsertAfter = 5;
	constexpr int valToInsert = 12345;

	BIEngine::ForwardList<ForwardListTestObj> list(valsToAdd);

	EXPECT_EQ(forwardListTestObjCnt, valsToAdd);

	{
		int i = 0;
		for (auto itr = list.Begin(); itr != list.End(); ++itr, ++i) {
			if (posToInsertAfter == i) {
				list.EmplaceAfter(itr, valToInsert);
				break;
			}
		}
	}

	EXPECT_EQ(forwardListTestObjCnt, valsToAdd + 1);

	{
		int i = 0;
		for (auto itr = list.CBegin(); itr != list.CEnd(); ++itr, ++i) {
			if (posToInsertAfter + 1 == i) {
				EXPECT_EQ(itr->val, valToInsert);
			}
			else {
				EXPECT_EQ(itr->val, 0);
			}
		}
		EXPECT_EQ(i, valsToAdd + 1);
	}
}


TEST(ForwardList, InsertAfter) {
	constexpr int valsToAdd = 10;
	constexpr int posToInsertAfter = 5;
	constexpr int valToInsert = 12345;

	BIEngine::ForwardList<int> list(valsToAdd);

	{
		int i = 0;
		for (auto itr = list.Begin(); itr != list.End(); ++itr, ++i) {
			if (posToInsertAfter == i) {
				list.InsertAfter(itr, valToInsert);
				break;
			}
		}
	}

	{
		int i = 0;
		for (auto itr = list.CBegin(); itr != list.CEnd(); ++itr, ++i) {
			if (posToInsertAfter + 1 == i) {
				EXPECT_EQ(*itr, valToInsert);
			}
			else {
				EXPECT_EQ(*itr, 0);
			}
		}
		EXPECT_EQ(i, valsToAdd + 1);
	}
}

TEST(ForwardList, EraseAfter) {
	constexpr int valsToAdd = 10;
	constexpr int posToEraseAfter = 5;

	BIEngine::ForwardList<int> list;

	auto itrToAdd = list.CBeforeBegin();
	for (int i = 0; i < valsToAdd; ++i) {
		itrToAdd = list.EmplaceAfter(itrToAdd, i);
	}

	{
		int i = 0;
		for (auto itr = list.Begin(); itr != list.End(); ++itr, ++i) {
			if (posToEraseAfter == i) {
				list.EraseAfter(itr);
				break;
			}
		}
	}

	{
		int i = 0;
		for (auto itr = list.CBegin(); itr != list.CEnd(); ++itr, ++i) {
			if (posToEraseAfter + 1 == i) {
				++i;
			}

			EXPECT_EQ(*itr, i);
		}
	}
}

TEST(ForwardList, SpliceAfter) {
	constexpr int valsToAdd = 10;
	constexpr int posToMoveAfter = 5;

	BIEngine::ForwardList<int> list;

	auto itrToAdd = list.CBeforeBegin();
	for (int i = 0; i < valsToAdd; ++i) {
		itrToAdd = list.EmplaceAfter(itrToAdd, i);
	}

	BIEngine::ForwardList<int> listMoveFrom;
	listMoveFrom.PushFront(100);
	listMoveFrom.PushFront(101);
	listMoveFrom.PushFront(102);

	{
		int i = 0;
		for (auto itr = list.Begin(); itr != list.End(); ++itr, ++i) {
			if (posToMoveAfter == i) {
				auto itrToReplaceAfter = listMoveFrom.Begin();

				list.SpliceAfter(itr, listMoveFrom, itrToReplaceAfter);
				break;
			}
		}
	}

	{
		int i = 0;
		int additionalNum = 0;
		for (auto itr = list.CBegin(); itr != list.CEnd(); ++itr, ++i) {
			if (posToMoveAfter + 1 == i) {
				EXPECT_EQ(*itr, 101);
				additionalNum = -1;
				continue;
			}

			EXPECT_EQ(*itr, i + additionalNum);
		}
	}

	{
		auto itr = listMoveFrom.CBegin();
		EXPECT_EQ(*itr, 102);
		++itr;
		EXPECT_EQ(*itr, 100);
		++itr;
		EXPECT_EQ(itr, listMoveFrom.CEnd());
	}
}