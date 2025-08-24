#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/SharedPtr.h"

class SharedPtrTestObj
{
public:
	SharedPtrTestObj(bool* wasDeleted = nullptr)
		: m_wasDeleted(wasDeleted)
	{

	}

	~SharedPtrTestObj()
	{
		if (m_wasDeleted == nullptr) {
			return;
		}

		*m_wasDeleted = true;
	}

	virtual void JustVirtualVoid()
	{

	}

public:
	int val = 0;

private:
	bool* m_wasDeleted = nullptr;
};

TEST(SharedPtr, AutoDelete) {
	bool wasDeleted = false;

	{
		BIEngine::SharedPtr<SharedPtrTestObj> ptr(new SharedPtrTestObj(&wasDeleted));
	}
	EXPECT_TRUE(wasDeleted);
}

TEST(SharedPtr, CtorFromSharedPtr) {
	bool wasDeleted = false;

	{
		BIEngine::SharedPtr<SharedPtrTestObj> ptr1(new SharedPtrTestObj(&wasDeleted));
		{
			BIEngine::SharedPtr<SharedPtrTestObj> ptr2(ptr1);
		}
		EXPECT_FALSE(wasDeleted);
	}
	EXPECT_TRUE(wasDeleted);
}

TEST(SharedPtr, CtorWithDeleter) {
	class SharedPtrTestObjDeleter
	{
	public:
		SharedPtrTestObjDeleter(bool* wasDeleterUsed)
			: m_pWasDeleterUsed(wasDeleterUsed)
		{

		}

		void operator()(SharedPtrTestObj* ptr)
		{
			delete ptr;
			*m_pWasDeleterUsed = true;
		}

	private:
		bool* m_pWasDeleterUsed;
	};

	bool wasDeleted = false;
	bool wasDeleterUsed = false;

	{
		BIEngine::SharedPtr<SharedPtrTestObj> ptr1(new SharedPtrTestObj(&wasDeleted), SharedPtrTestObjDeleter(&wasDeleterUsed));
		{
			BIEngine::SharedPtr<SharedPtrTestObj> ptr2(ptr1);
		}
		EXPECT_FALSE(wasDeleted);
		EXPECT_FALSE(wasDeleterUsed);
	}
	EXPECT_TRUE(wasDeleted);
	EXPECT_TRUE(wasDeleterUsed);
}

TEST(SharedPtr, AssignmentOperator) {
	bool wasDeleted1 = false;
	bool wasDeleted2 = false;
	{
		BIEngine::SharedPtr<SharedPtrTestObj> ptr1(new SharedPtrTestObj(&wasDeleted1));
		{
			BIEngine::SharedPtr<SharedPtrTestObj> ptr2(new SharedPtrTestObj(&wasDeleted2));
			ptr2 = ptr1;
			EXPECT_TRUE(wasDeleted2);
		}
		EXPECT_FALSE(wasDeleted1);
	}
	EXPECT_TRUE(wasDeleted1);
}


TEST(SharedPtr, BoolOperator) {
	{
		BIEngine::SharedPtr<int> ptr;
		EXPECT_FALSE(ptr);
	}

	{
		BIEngine::SharedPtr<int> ptr(new int());
		EXPECT_TRUE(ptr);
	}
}

TEST(SharedPtr, MoveCtor) {
	bool wasDeleted = false;
	BIEngine::SharedPtr<SharedPtrTestObj> ptr(new SharedPtrTestObj(&wasDeleted));
	EXPECT_TRUE(ptr);

	{
		BIEngine::SharedPtr<SharedPtrTestObj> ptrMoveTo(std::move(ptr));
		EXPECT_TRUE(ptrMoveTo);
		EXPECT_FALSE(wasDeleted);
	}

	EXPECT_FALSE(ptr);
	EXPECT_TRUE(wasDeleted);
}

TEST(SharedPtr, Get) {
	int* rawPtr = new int(10);
	BIEngine::SharedPtr<int> ptr(rawPtr);
	EXPECT_EQ(*ptr.Get(), 10);

	*ptr.Get() = 20;

	EXPECT_EQ(*ptr.Get(), 20);
}

TEST(SharedPtr, OperatorStar) {
	int* rawPtr = new int(10);
	BIEngine::SharedPtr<int> ptr(rawPtr);
	EXPECT_EQ(*ptr, 10);

	*ptr = 20;

	EXPECT_EQ(*ptr, 20);
}

TEST(SharedPtr, ResetEmptyArgument) {
	bool wasDeleted = false;
	BIEngine::SharedPtr<SharedPtrTestObj> ptr(new SharedPtrTestObj(&wasDeleted));

	ptr.Reset();

	EXPECT_FALSE(ptr);
	EXPECT_TRUE(wasDeleted);
}


TEST(SharedPtr, ResetWithPtr) {
	bool wasDeleted = false;
	BIEngine::SharedPtr<SharedPtrTestObj> ptr(new SharedPtrTestObj(&wasDeleted));

	bool dummy = false;
	SharedPtrTestObj* newRawPtr = new SharedPtrTestObj(&dummy);

	ptr.Reset(newRawPtr);
	ptr->val = 12;

	EXPECT_TRUE(wasDeleted);
	EXPECT_FALSE(dummy);
	EXPECT_EQ(newRawPtr->val, 12);
}

TEST(SharedPtr, MakeShared) {
	bool wasDeleted = false;

	{
		BIEngine::SharedPtr<SharedPtrTestObj> ptr = BIEngine::MakeShared<SharedPtrTestObj>(&wasDeleted);
		EXPECT_FALSE(wasDeleted);
	}

	EXPECT_TRUE(wasDeleted);
}

class SharedPtrInhTestObj : public SharedPtrTestObj
{
public:
	SharedPtrInhTestObj(int val, bool* wasDeleted = nullptr)
		: SharedPtrTestObj(wasDeleted), val2(val)
	{

	}

public:
	int val2 = 0;
};

TEST(SharedPtr, StaticPointerCast) {
	constexpr int testVal = 10;

	bool wasDeleted = false;

	{
		BIEngine::SharedPtr<SharedPtrInhTestObj> ptr2;
		{
			BIEngine::SharedPtr<SharedPtrTestObj> ptr = BIEngine::MakeShared<SharedPtrInhTestObj>(testVal, &wasDeleted);
			EXPECT_FALSE(wasDeleted);
			ptr2 = BIEngine::StaticPointerCast<SharedPtrInhTestObj>(ptr);
		}
		EXPECT_FALSE(wasDeleted);
		EXPECT_EQ(ptr2->val2, testVal);
	}


	EXPECT_TRUE(wasDeleted);
}

TEST(SharedPtr, StaticPointerCastMove) {
	constexpr int testVal = 10;

	bool wasDeleted = false;

	{
		BIEngine::SharedPtr<SharedPtrInhTestObj> ptr2;
		{
			BIEngine::SharedPtr<SharedPtrTestObj> ptr = BIEngine::MakeShared<SharedPtrInhTestObj>(testVal, &wasDeleted);
			EXPECT_FALSE(wasDeleted);
			ptr2 = BIEngine::StaticPointerCast<SharedPtrInhTestObj>(std::move(ptr));
		}
		EXPECT_FALSE(wasDeleted);
		EXPECT_EQ(ptr2->val2, testVal);
	}


	EXPECT_TRUE(wasDeleted);
}

class SharedPtrTestDummy : public SharedPtrInhTestObj
{

};

TEST(SharedPtr, DynamicPointerCast) {
	constexpr int testVal = 10;

	bool wasDeleted = false;

	{
		BIEngine::SharedPtr<SharedPtrInhTestObj> ptr2;
		{
			BIEngine::SharedPtr<SharedPtrTestObj> ptr = BIEngine::MakeShared<SharedPtrInhTestObj>(testVal, &wasDeleted);
			EXPECT_FALSE(wasDeleted);
			ptr2 = BIEngine::DynamicPointerCast<SharedPtrInhTestObj>(ptr);

			BIEngine::SharedPtr<SharedPtrTestDummy> ptr3 = BIEngine::DynamicPointerCast<SharedPtrTestDummy>(ptr);
			EXPECT_FALSE(ptr3);
		}
		EXPECT_FALSE(wasDeleted);
		EXPECT_EQ(ptr2->val2, testVal);
	}


	EXPECT_TRUE(wasDeleted);
}

TEST(SharedPtr, DynamicPointerCastMove) {
	constexpr int testVal = 10;

	bool wasDeleted = false;

	{
		BIEngine::SharedPtr<SharedPtrInhTestObj> ptr2;
		{
			BIEngine::SharedPtr<SharedPtrTestObj> ptr = BIEngine::MakeShared<SharedPtrInhTestObj>(testVal, &wasDeleted);
			EXPECT_FALSE(wasDeleted);
			ptr2 = BIEngine::DynamicPointerCast<SharedPtrInhTestObj>(std::move(ptr));

			BIEngine::SharedPtr<SharedPtrTestDummy> ptr3 = BIEngine::DynamicPointerCast<SharedPtrTestDummy>(std::move(ptr));
			EXPECT_FALSE(ptr);
			EXPECT_FALSE(ptr3);
		}
		EXPECT_FALSE(wasDeleted);
		EXPECT_EQ(ptr2->val2, testVal);
	}


	EXPECT_TRUE(wasDeleted);
}
