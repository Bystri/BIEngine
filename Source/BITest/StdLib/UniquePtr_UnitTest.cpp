#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/UniquePtr.h"

class UniqeuePtrTestObj
{
public:
	UniqeuePtrTestObj(bool& wasDeleted)
		: m_wasDeleted(wasDeleted)
	{

	}

	~UniqeuePtrTestObj()
	{
		m_wasDeleted = true;
	}

public:
	int val = 0;

private:
	bool& m_wasDeleted;
};

TEST(UniquePtr, AutoDelete) {
	bool wasDeleted = false;

	{
		BIEngine::UniquePtr<UniqeuePtrTestObj> ptr(new UniqeuePtrTestObj(wasDeleted));
	}
	EXPECT_TRUE(wasDeleted);
}

TEST(UniquePtr, BoolOperator) {
	{
		BIEngine::UniquePtr<int> ptr;
		EXPECT_FALSE(ptr);
	}

	{
		BIEngine::UniquePtr<int> ptr(new int());
		EXPECT_TRUE(ptr);
	}
}

TEST(UniquePtr, MoveCtor) {
	bool wasDeleted = false;
	BIEngine::UniquePtr<UniqeuePtrTestObj> ptr(new UniqeuePtrTestObj(wasDeleted));
	EXPECT_TRUE(ptr);

	{
		BIEngine::UniquePtr<UniqeuePtrTestObj> ptrMoveTo(std::move(ptr));
		EXPECT_TRUE(ptrMoveTo);
		EXPECT_FALSE(wasDeleted);
	}

	EXPECT_FALSE(ptr);
	EXPECT_TRUE(wasDeleted);
}

TEST(UniquePtr, Get) {
	int* rawPtr = new int(10);
	BIEngine::UniquePtr<int> ptr(rawPtr);
	EXPECT_EQ(*ptr.Get(), 10);

	*ptr.Get() = 20;

	EXPECT_EQ(*ptr.Get(), 20);
}

TEST(UniquePtr, OperatorStar) {
	int* rawPtr = new int(10);
	BIEngine::UniquePtr<int> ptr(rawPtr);
	EXPECT_EQ(*ptr, 10);

	*ptr = 20;

	EXPECT_EQ(*ptr, 20);
}

TEST(UniquePtr, Release) {
	BIEngine::UniquePtr<int> ptr(new int(10));
	EXPECT_EQ(*ptr, 10);

	int* rawPtr = ptr.Release();

	EXPECT_EQ(*rawPtr, 10);

	EXPECT_FALSE(ptr);
}

TEST(UniquePtr, Reset) {
	bool wasDeleted = false;
	BIEngine::UniquePtr<UniqeuePtrTestObj> ptr(new UniqeuePtrTestObj(wasDeleted));

	bool dummy = false;
	UniqeuePtrTestObj* newRawPtr = new UniqeuePtrTestObj(dummy);

	ptr.Reset(newRawPtr);
	ptr->val = 12;

	EXPECT_TRUE(wasDeleted);
	EXPECT_FALSE(dummy);
	EXPECT_EQ(newRawPtr->val, 12);
}

TEST(UniquePtr, MakeUnique) {
	bool wasDeleted = false;

	{
		BIEngine::UniquePtr<UniqeuePtrTestObj> ptr = BIEngine::MakeUnique<UniqeuePtrTestObj>(wasDeleted);
		EXPECT_FALSE(wasDeleted);
	}

	EXPECT_TRUE(wasDeleted);
}
