#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/Deque.h"

TEST(Deque, PushBack) {
	constexpr int valsToAdd = 100;

	BIEngine::Deque<int> deque;

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushBack(i);
	}

	for (int i = 0; i < valsToAdd; ++i) {
		EXPECT_EQ(deque[i], i);
	}
}

TEST(Deque, PushFront) {
	constexpr int valsToAdd = 100;

	BIEngine::Deque<int> deque;

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushFront(i);
	}

	for (int i = 0; i < valsToAdd; ++i) {
		EXPECT_EQ(deque[i], valsToAdd - i - 1);
	}
}

TEST(Deque, PushFrontAndPushBack) {
	constexpr int valsToAdd = 100;

	BIEngine::Deque<int> deque;

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushFront(i);
	}

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushBack(i);
	}

	for (int i = 0; i < valsToAdd; ++i) {
		EXPECT_EQ(deque[i], valsToAdd - i - 1);
	}

	for (int i = 0; i < valsToAdd; ++i) {
		EXPECT_EQ(deque[i + valsToAdd], i);
	}
}

TEST(Deque, FrontAndBack) {
	constexpr int valsToAdd = 100;

	BIEngine::Deque<int> deque;

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushBack(i);
	}

	EXPECT_EQ(deque.Front(), 0);
	EXPECT_EQ(deque.Back(), valsToAdd - 1);
}

TEST(Deque, ConstIterator) {
	constexpr int valsToAdd = 100;

	BIEngine::Deque<int> deque;

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushBack(i);
	}

	{
		int i = 0;
		for (auto itr = deque.CBegin(); itr != deque.CEnd(); ++itr, ++i) {
			EXPECT_EQ(*itr, i);
		}
	}

	{
		auto itr = deque.CEnd();
		--itr;
		for (int i = 0; i < valsToAdd; --itr, ++i) {
			EXPECT_EQ(*itr, valsToAdd - i - 1);
		}
	}
}

TEST(Deque, Iterator) {
	constexpr int valsToAdd = 100;

	BIEngine::Deque<int> deque;

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushBack(i);
	}

	{
		int i = valsToAdd - 1;
		for (auto itr = deque.Begin(); itr != deque.End(); ++itr, --i) {
			*itr = i;
		}
	}

	{
		int i = 0;
		for (auto itr = deque.Begin(); itr != deque.End(); ++itr, ++i) {
			EXPECT_EQ(*itr, valsToAdd - i - 1);
		}
	}
}

TEST(Deque, ConstReverseIterator) {
	constexpr int valsToAdd = 100;

	BIEngine::Deque<int> deque;

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushBack(i);
	}

	{
		int i = 0;
		for (auto itr = deque.CRBegin(); itr != deque.CREnd(); ++itr, ++i) {
			EXPECT_EQ(*itr, valsToAdd - i - 1);
		}
	}
}

TEST(Deque, ReverseIterator) {
	constexpr int valsToAdd = 100;

	BIEngine::Deque<int> deque;

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushBack(i);
	}

	{
		int i = 0;
		for (auto itr = deque.RBegin(); itr != deque.REnd(); ++itr, ++i) {
			EXPECT_EQ(*itr, valsToAdd - i - 1);
			*itr = i;
		}
	}

	for (int i = 0; i < valsToAdd; ++i) {
		EXPECT_EQ(deque[i], valsToAdd - i - 1);
	}
}

TEST(Deque, CtroN) {
	constexpr int valsToAdd = 100;

	BIEngine::Deque<int> deque(valsToAdd);

	EXPECT_EQ(deque.Size(), valsToAdd);

	{
		int i = 0;
		for (auto itr = deque.CBegin(); itr != deque.CEnd(); ++itr, ++i) {
			EXPECT_EQ(*itr, int());
		}

		EXPECT_EQ(i, valsToAdd);
	}
}

TEST(Deque, CtroNWithVal) {
	constexpr int valsToAdd = 100;
	constexpr int val = 5;

	BIEngine::Deque<int> deque(valsToAdd, val);

	EXPECT_EQ(deque.Size(), valsToAdd);

	{
		int i = 0;
		for (auto itr = deque.CBegin(); itr != deque.CEnd(); ++itr, ++i) {
			EXPECT_EQ(*itr, val);
		}

		EXPECT_EQ(i, valsToAdd);
	}
}

TEST(Deque, CopyCtor) {
	constexpr int valsToAdd = 100;

	BIEngine::Deque<int> deque;

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushBack(i);
	}

	{
		BIEngine::Deque<int> dequeToCopy(deque);

		EXPECT_EQ(dequeToCopy.Size(), valsToAdd);

		int i = 0;
		for (auto itr = dequeToCopy.CBegin(); itr != dequeToCopy.CEnd(); ++itr, ++i) {
			EXPECT_EQ(*itr, i);
		}

		EXPECT_EQ(i, valsToAdd);
	}
}

TEST(Deque, MoveCtor) {
	constexpr int valsToAdd = 100;

	BIEngine::Deque<int> deque;

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushBack(i);
	}

	{
		BIEngine::Deque<int> dequeToCopy(std::move(deque));

		EXPECT_EQ(dequeToCopy.Size(), valsToAdd);

		int i = 0;
		for (auto itr = dequeToCopy.CBegin(); itr != dequeToCopy.CEnd(); ++itr, ++i) {
			EXPECT_EQ(*itr, i);
		}

		EXPECT_EQ(i, valsToAdd);
	}
}

TEST(Deque, CopyAssignment) {
	constexpr int valsToAdd = 100;

	BIEngine::Deque<int> deque;

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushBack(i);
	}

	{
		BIEngine::Deque<int> dequeToCopy;

		for (int i = 0; i < valsToAdd; ++i) {
			dequeToCopy.PushBack(valsToAdd - i - 1);
		}

		dequeToCopy = deque;

		EXPECT_EQ(dequeToCopy.Size(), valsToAdd);

		int i = 0;
		for (auto itr = dequeToCopy.CBegin(); itr != dequeToCopy.CEnd(); ++itr, ++i) {
			EXPECT_EQ(*itr, i);
		}

		EXPECT_EQ(i, valsToAdd);
	}
}

TEST(Deque, MoveAssignment) {
	constexpr int valsToAdd = 100;

	BIEngine::Deque<int> dequeToCopy;

	{

		BIEngine::Deque<int> deque;

		for (int i = 0; i < valsToAdd; ++i) {
			deque.PushBack(i);
		}



		for (int i = 0; i < valsToAdd; ++i) {
			dequeToCopy.PushBack(valsToAdd - i - 1);
		}

		dequeToCopy = std::move(deque);
	}

	EXPECT_EQ(dequeToCopy.Size(), valsToAdd);

	int i = 0;
	for (auto itr = dequeToCopy.CBegin(); itr != dequeToCopy.CEnd(); ++itr, ++i) {
		EXPECT_EQ(*itr, i);
	}

	EXPECT_EQ(i, valsToAdd);
}

TEST(Deque, EmplaceBack) {
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

	BIEngine::Deque<EmplaceBackTestObj> deque;

	deque.EmplaceBack(1, 2);

	EXPECT_EQ(deque[0].a, 1);
	EXPECT_EQ(deque[0].b, 2);
}

TEST(Deque, EmplaceFront) {
	struct EmplaceFrontTestObj
	{
		EmplaceFrontTestObj(int _a, int _b)
			: a(_a)
			, b(_b)
		{

		}

		int a;
		int b;
	};

	BIEngine::Deque<EmplaceFrontTestObj> deque;

	deque.EmplaceFront(1, 2);

	EXPECT_EQ(deque[0].a, 1);
	EXPECT_EQ(deque[0].b, 2);
}


TEST(Deque, PopBack) {
	constexpr int valsToAdd = 100;
	constexpr int valsToPop = 50;

	BIEngine::Deque<int> deque;

	EXPECT_EQ(deque.Size(), 0);
	EXPECT_TRUE(deque.Empty());

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushBack(i);
	}

	for (int i = 0; i < valsToPop; ++i) {
		deque.PopBack();
	}

	constexpr int remainingElements = valsToAdd - valsToPop;

	EXPECT_EQ(deque.Size(), remainingElements);
	EXPECT_FALSE(deque.Empty());

	for (int i = 0; i < remainingElements; ++i) {
		EXPECT_EQ(deque[i], i);
	}

	for (int i = 0; i < remainingElements; ++i) {
		deque.PopBack();
	}

	EXPECT_EQ(deque.Size(), 0);
	EXPECT_TRUE(deque.Empty());
}

TEST(Deque, PopFront) {
	constexpr int valsToAdd = 100;
	constexpr int valsToPop = 50;

	BIEngine::Deque<int> deque;

	EXPECT_EQ(deque.Size(), 0);
	EXPECT_TRUE(deque.Empty());

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushFront(i);
	}

	for (int i = 0; i < valsToPop; ++i) {
		deque.PopFront();
	}

	constexpr int remainingElements = valsToAdd - valsToPop;

	EXPECT_EQ(deque.Size(), remainingElements);
	EXPECT_FALSE(deque.Empty());

	for (int i = 0; i < remainingElements; ++i) {
		EXPECT_EQ(deque[i], remainingElements - i - 1);
	}

	for (int i = 0; i < remainingElements; ++i) {
		deque.PopBack();
	}

	EXPECT_EQ(deque.Size(), 0);
	EXPECT_TRUE(deque.Empty());
}

TEST(Deque, PushAndPopCombination) {
	constexpr int valsToAdd = 100;

	BIEngine::Deque<int> deque;

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushFront(i);
	}

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushBack(i);
	}

	for (int i = 0; i < valsToAdd; ++i) {
		EXPECT_EQ(deque[i], valsToAdd - i - 1);
	}

	for (int i = 0; i < valsToAdd; ++i) {
		EXPECT_EQ(deque[i + valsToAdd], i);
	}

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PopFront();
	}

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PopBack();
	}

	EXPECT_EQ(deque.Size(), 0);

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushFront(i);
	}

	for (int i = 0; i < valsToAdd; ++i) {
		deque.PushBack(i);
	}

	for (int i = 0; i < valsToAdd; ++i) {
		EXPECT_EQ(deque[i], valsToAdd - i - 1);
	}

	for (int i = 0; i < valsToAdd; ++i) {
		EXPECT_EQ(deque[i + valsToAdd], i);
	}
}

TEST(Deque, ResizeGrow) {
	constexpr int valsNum = 100;
	constexpr int valsAfterResizeNum = 200;

	BIEngine::Deque<int> deque;

	for (int i = 0; i < valsNum; ++i) {
		deque.PushBack(i);
	}

	deque.Resize(valsAfterResizeNum);

	EXPECT_EQ(deque.Size(), valsAfterResizeNum);

	for (int i = 0; i < valsAfterResizeNum; ++i) {
		if (i >= valsNum) {
			EXPECT_EQ(deque[i], int());
			continue;
		}

		EXPECT_EQ(deque[i], i);
	}
}

TEST(Deque, ResizeShrink) {
	constexpr int valsNum = 100;
	constexpr int valsAfterResizeNum = 5;

	BIEngine::Deque<int> deque;

	for (int i = 0; i < valsNum; ++i) {
		deque.PushBack(i);
	}

	deque.Resize(valsAfterResizeNum);

	EXPECT_EQ(deque.Size(), valsAfterResizeNum);

	for (int i = 0; i < valsAfterResizeNum; ++i) {
		EXPECT_EQ(deque[i], i);
	}
}
