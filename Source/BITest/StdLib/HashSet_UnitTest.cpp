#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/HashSet.h"


TEST(HashSet, Insert) {
	BIEngine::HashSet<int> set;

	const int valsToAdd[] = { 10, 2, 4, 5 };

	for (const auto& val : valsToAdd) {
		set.Insert(val);
	}
	
	constexpr int valsToAddNum = std::size(valsToAdd);
	bool hitTable[valsToAddNum];
	for (int i = 0; i < valsToAddNum; ++i) {
		hitTable[i] = false;
	}

	for (const int& val : set) {
		for (int i = 0; i < valsToAddNum; ++i) {
			if (val == valsToAdd[i]) {
				hitTable[i] = true;
				break;
			}
		}
	}

	for (int i = 0; i < valsToAddNum; ++i) {
		EXPECT_TRUE(hitTable[i]);
	}
}

TEST(HashSet, Clear) {
	BIEngine::HashSet<int> set;

	const int valsToAdd[] = { 10, 2, 4, 5 };

	for (const auto& val : valsToAdd) {
		set.Insert(val);
	}

	EXPECT_EQ(set.Size(), std::size(valsToAdd));
	set.Clear();
	EXPECT_EQ(set.Size(), 0);

	int elementIterated = 0;
	for (auto itr = set.Begin(); itr != set.End(); ++itr) {
		++elementIterated;
	}

	EXPECT_EQ(elementIterated, 0);
}


static int setTestObjCnt = 0;
static bool canCntDestructor = false;

class HashSetTestObj
{
public:
	HashSetTestObj()
	{
		++setTestObjCnt;
	}

	HashSetTestObj(int v)
		: val(v)
	{
		++setTestObjCnt;
	}

	~HashSetTestObj()
	{
		if (!canCntDestructor) {
			return;
		}

		--setTestObjCnt;
	}

	int val = 0;
};

class HashSetTestObjHash
{
public:
	int operator()(const HashSetTestObj& obj)
	{
		return std::hash<int>()(obj.val);
	}
};


class HashSetTestObjEqualTo
{
public:
	bool operator()(const HashSetTestObj& lhs, const HashSetTestObj& rhs)
	{
		return lhs.val == rhs.val;
	}
};

TEST(HashSet, Emplace) {

	{
		BIEngine::HashSet<HashSetTestObj, HashSetTestObjHash, HashSetTestObjEqualTo> set;

		const int valsToAdd[] = { 10, 2, 4, 5 };

		for (const auto& val : valsToAdd) {
			set.Emplace(val);
		}

		EXPECT_EQ(setTestObjCnt, std::size(valsToAdd));

		constexpr int valsToAddNum = std::size(valsToAdd);
		bool hitTable[valsToAddNum];
		for (int i = 0; i < valsToAddNum; ++i) {
			hitTable[i] = false;
		}

		for (const auto& val : set) {
			for (int i = 0; i < valsToAddNum; ++i) {
				if (val.val == valsToAdd[i]) {
					hitTable[i] = true;
					break;
				}
			}
		}

		for (int i = 0; i < valsToAddNum; ++i) {
			EXPECT_TRUE(hitTable[i]);
		}

		canCntDestructor = true;
	}

	EXPECT_EQ(setTestObjCnt, 0);
}


TEST(HashSet, EraseWithKey) {
	BIEngine::HashSet<int, int> set;

	const int valsToAdd[] = { 10, 2, 4, 5 };
	constexpr int idxKeyToDelete = 1;

	for (const auto& val : valsToAdd) {
		set.Insert(val);
	}

	set.Erase(valsToAdd[idxKeyToDelete]);

	constexpr int valsToAddNum = std::size(valsToAdd);
	bool hitTable[valsToAddNum];
	for (int i = 0; i < valsToAddNum; ++i) {
		hitTable[i] = false;
	}

	for (const int& val : set) {
		for (int i = 0; i < valsToAddNum; ++i) {
			if (val == valsToAdd[i]) {
				hitTable[i] = true;
				break;
			}
		}
	}

	for (int i = 0; i < valsToAddNum; ++i) {
		if (i == idxKeyToDelete) {
			EXPECT_FALSE(hitTable[i]);
			continue;
		}

		EXPECT_TRUE(hitTable[i]);
	}
}

TEST(HashSet, EraseWithItr) {
	BIEngine::HashSet<int, int> set;

	const int valsToAdd[] = { 10, 2, 4, 5 };
	constexpr int idxKeyToDelete = 1;

	for (const auto& val : valsToAdd) {
		set.Insert(val);
	}

	constexpr int valsToAddNum = std::size(valsToAdd);
	bool hitTable[valsToAddNum];
	for (int i = 0; i < valsToAddNum; ++i) {
		hitTable[i] = false;
	}

	for (auto itr = set.Begin(); itr != set.End();) {
		for (int i = 0; i < valsToAddNum; ++i) {
			if (*itr == valsToAdd[i]) {
				hitTable[i] = true;
				break;
			}
		}

		if (*itr == valsToAdd[idxKeyToDelete]) {
			itr = set.Erase(itr);
			continue;
		}

		++itr;
	}

	for (int i = 0; i < valsToAddNum; ++i) {
		EXPECT_TRUE(hitTable[i]);
	}

	for (int i = 0; i < valsToAddNum; ++i) {
		hitTable[i] = false;
	}

	for (const int& val : set) {
		for (int i = 0; i < valsToAddNum; ++i) {
			if (val == valsToAdd[i]) {
				hitTable[i] = true;
				break;
			}
		}
	}

	for (int i = 0; i < valsToAddNum; ++i) {
		if (i == idxKeyToDelete) {
			EXPECT_FALSE(hitTable[i]);
			continue;
		}

		EXPECT_TRUE(hitTable[i]);
	}
}

TEST(HashSet, Rehashing) {
	BIEngine::HashSet<int> set;

	const int valsToAdd[] = { 10, 2, 102, 11, 8, 111, 25, 32521, 86, 100, 254, 37, 12, 1000, 24 };

	bool wasRehashed = false;
	float prevLoadFactor = 0.0f;
	for (const auto& val : valsToAdd) {
		set.Insert(val);
		if (prevLoadFactor > set.LoadFactor()) {
			wasRehashed = true;
		}

		prevLoadFactor = set.LoadFactor();
	}

	EXPECT_EQ(set.Size(), std::size(valsToAdd));
	EXPECT_TRUE(wasRehashed);

	bool hitTable[std::size(valsToAdd)];
	for (int i = 0; i < std::size(hitTable); ++i) {
		hitTable[i] = false;
	}

	for (auto itr = set.Begin(); itr != set.End(); ++itr) {
		for (int i = 0; i < std::size(hitTable); ++i) {
			if (*itr == valsToAdd[i]) {
				hitTable[i] = true;
				break;
			}
		}
	}

	for (int i = 0; i < std::size(hitTable); ++i) {
		EXPECT_TRUE(hitTable[i]);
	}
}

TEST(HashSet, Find) {
	BIEngine::HashSet<int> set;

	const int valsToAdd[] = { 10, 2, 4, 5 };
	constexpr int idxKeyToFindAndChange = 1;
	constexpr int notPresentedVal = 2222;

	for (const auto& val : valsToAdd) {
		set.Insert(val);
	}

	auto findItr = set.Find(valsToAdd[idxKeyToFindAndChange]);
	EXPECT_NE(findItr, set.End());

	auto findNotPresentedItr = set.Find(notPresentedVal);
	EXPECT_EQ(findNotPresentedItr, set.End());
}