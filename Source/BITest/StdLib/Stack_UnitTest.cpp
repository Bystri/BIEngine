#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/Stack.h"

TEST(Stack, Push) {
	BIEngine::Stack<int> stack;

	EXPECT_TRUE(stack.Empty());
	EXPECT_EQ(stack.Size(), 0);

	constexpr int testVal = 10;
	stack.Push(testVal);

	EXPECT_FALSE(stack.Empty());
	EXPECT_EQ(stack.Size(), 1);
	EXPECT_EQ(stack.Top(), testVal);
}

TEST(Stack, Pop) {
	BIEngine::Stack<int> stack;

	EXPECT_TRUE(stack.Empty());
	EXPECT_EQ(stack.Size(), 0);

	constexpr int testVal1 = 10;
	constexpr int testVal2 = 20;
	stack.Push(testVal1);

	EXPECT_EQ(stack.Top(), testVal1);

	stack.Push(testVal2);

	EXPECT_FALSE(stack.Empty());
	EXPECT_EQ(stack.Size(), 2);
	EXPECT_EQ(stack.Top(), testVal2);

	stack.Pop();

	EXPECT_FALSE(stack.Empty());
	EXPECT_EQ(stack.Size(), 1);
	EXPECT_EQ(stack.Top(), testVal1);

	stack.Pop();

	EXPECT_TRUE(stack.Empty());
	EXPECT_EQ(stack.Size(), 0);
}

bool wasMoved = false;
int cntCtor = 0;
int cntDestr = 0;

static void ResetTestObjVals()
{
	wasMoved = false;
	cntCtor = 0;
	cntDestr = 0;
}

class StackTestObj
{
public:
	StackTestObj(int val) : val(val) 
	{
		++cntCtor;
	}

	StackTestObj(const StackTestObj& rhs) : val(rhs.val)
	{
		++cntCtor;
	}

	StackTestObj(StackTestObj&& rhs) : val(rhs.val) 
	{
		wasMoved = true;
		++cntCtor;
	}

	~StackTestObj()
	{
		++cntDestr;
	}

	int val;
};

TEST(Stack, PushMove) {
	ResetTestObjVals();

	BIEngine::Stack<StackTestObj> stack;

	EXPECT_TRUE(stack.Empty());
	EXPECT_EQ(stack.Size(), 0);

	constexpr int testVal = 10;
	StackTestObj obj(testVal);

	stack.Push(std::move(obj));

	EXPECT_TRUE(wasMoved);
	EXPECT_FALSE(stack.Empty());
	EXPECT_EQ(stack.Size(), 1);
	EXPECT_EQ(stack.Top().val, testVal);
}

TEST(Stack, Emplace) {
	ResetTestObjVals();

	{
		BIEngine::Stack<StackTestObj> stack;

		EXPECT_TRUE(stack.Empty());
		EXPECT_EQ(stack.Size(), 0);

		constexpr int testVal = 10;

		stack.Emplace(testVal);

		EXPECT_EQ(cntCtor, 1);
		EXPECT_FALSE(stack.Empty());
		EXPECT_EQ(stack.Size(), 1);
		EXPECT_EQ(stack.Top().val, testVal);
	}

	EXPECT_EQ(cntDestr, 1);
}