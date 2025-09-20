#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/Queue.h"

TEST(Queue, Push) {
	BIEngine::Queue<int> queue;

	EXPECT_TRUE(queue.Empty());
	EXPECT_EQ(queue.Size(), 0);

	constexpr int testVal = 10;
	queue.Push(testVal);

	EXPECT_FALSE(queue.Empty());
	EXPECT_EQ(queue.Size(), 1);
	EXPECT_EQ(queue.Front(), testVal);
	EXPECT_EQ(queue.Back(), testVal);
}

TEST(Queue, Pop) {
	BIEngine::Queue<int> queue;

	EXPECT_TRUE(queue.Empty());
	EXPECT_EQ(queue.Size(), 0);

	constexpr int testVal1 = 10;
	constexpr int testVal2 = 20;
	queue.Push(testVal1);

	EXPECT_EQ(queue.Front(), testVal1);

	queue.Push(testVal2);

	EXPECT_FALSE(queue.Empty());
	EXPECT_EQ(queue.Size(), 2);
	EXPECT_EQ(queue.Front(), testVal1);
	EXPECT_EQ(queue.Back(), testVal2);

	queue.Pop();

	EXPECT_FALSE(queue.Empty());
	EXPECT_EQ(queue.Size(), 1);
	EXPECT_EQ(queue.Front(), testVal2);
	EXPECT_EQ(queue.Back(), testVal2);

	queue.Pop();

	EXPECT_TRUE(queue.Empty());
	EXPECT_EQ(queue.Size(), 0);
}

static bool wasMoved = false;
static int cntCtor = 0;
static int cntDestr = 0;

static void ResetTestObjVals()
{
	wasMoved = false;
	cntCtor = 0;
	cntDestr = 0;
}

class QueueTestObj
{
public:
	QueueTestObj(int val) : val(val)
	{
		++cntCtor;
	}

	QueueTestObj(const QueueTestObj& rhs) : val(rhs.val)
	{
		++cntCtor;
	}

	QueueTestObj(QueueTestObj&& rhs) : val(rhs.val)
	{
		wasMoved = true;
		++cntCtor;
	}

	~QueueTestObj()
	{
		++cntDestr;
	}

	int val;
};

TEST(Queue, PushMove) {
	ResetTestObjVals();

	BIEngine::Queue<QueueTestObj> queue;

	EXPECT_TRUE(queue.Empty());
	EXPECT_EQ(queue.Size(), 0);

	constexpr int testVal = 10;
	QueueTestObj obj(testVal);

	queue.Push(std::move(obj));

	EXPECT_TRUE(wasMoved);
	EXPECT_FALSE(queue.Empty());
	EXPECT_EQ(queue.Size(), 1);
	EXPECT_EQ(queue.Front().val, testVal);
}

TEST(Queue, Emplace) {
	ResetTestObjVals();

	{
		BIEngine::Queue<QueueTestObj> queue;

		EXPECT_TRUE(queue.Empty());
		EXPECT_EQ(queue.Size(), 0);

		constexpr int testVal = 10;

		queue.Emplace(testVal);

		EXPECT_EQ(cntCtor, 1);
		EXPECT_FALSE(queue.Empty());
		EXPECT_EQ(queue.Size(), 1);
		EXPECT_EQ(queue.Front().val, testVal);
	}

	EXPECT_EQ(cntDestr, 1);
}