#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/HashMap.h"

TEST(HashMap, Insert) {
	BIEngine::HashMap<int, int> map;

	map.Insert(10, 2);
	EXPECT_EQ(map[10], 2);
}

TEST(HashMap, Clear) {
	BIEngine::HashMap<int, int> map;

	const std::pair<int, int> keysAndVals[] = { {10, 2}, {100, 8}, {25, 11} };

	for (const auto& keyAndVal : keysAndVals) {
		map.Insert(keyAndVal.first, keyAndVal.second);
	}

	EXPECT_EQ(map.Size(), std::size(keysAndVals));
	map.Clear();
	EXPECT_EQ(map.Size(), 0);

	int elementIterated = 0;
	for (auto itr = map.Begin(); itr != map.End(); ++itr) {
		++elementIterated;
	}

	EXPECT_EQ(elementIterated, 0);
}

TEST(HashMap, Iterator_Iteration) {
	BIEngine::HashMap<int, int> map;

	const std::pair<int, int> keysAndVals[] = { {10, 2}, {100, 8}, {25, 11} };

	for (const auto& keyAndVal : keysAndVals) {
		map.Insert(keyAndVal.first, keyAndVal.second);
	}

	bool hitTable[3];
	for (int i = 0; i < 3; ++i) {
		hitTable[i] = false;
	}

	for (auto itr = map.Begin(); itr != map.End(); ++itr) {
		for (int i = 0; i < 3; ++i) {
			if (itr->first == keysAndVals[i].first && itr->second == keysAndVals[i].second) {
				hitTable[i] = true;
				break;
			}
		}
	}

	for (int i = 0; i < 3; ++i) {
		EXPECT_TRUE(hitTable[i]);
	}
}

TEST(HashMap, Iterator_ChangeVal) {
	BIEngine::HashMap<int, int> map;

	const std::pair<int, int> keysAndVals[] = { {10, 2}, {100, 8}, {25, 11} };

	for (const auto& keyAndVal : keysAndVals) {
		map.Insert(keyAndVal.first, keyAndVal.second);
	}

	const int newVals[] = { 11, 18, 24 };

	for (int i = 0; i < 3; ++i) {
		for (auto itr = map.Begin(); itr != map.End(); ++itr) {
			if (itr->first == keysAndVals[i].first) {
				itr->second = newVals[i];
				break;
			}
		}
	}

	for (int i = 0; i < 3; ++i) {
		EXPECT_EQ(map[keysAndVals[i].first], newVals[i]);
	}
}

TEST(HashMap, ConstIterator) {
	BIEngine::HashMap<int, int> map;

	const std::pair<int, int> keysAndVals[] = { {10, 2}, {100, 8}, {25, 11} };

	for (const auto& keyAndVal : keysAndVals) {
		map.Insert(keyAndVal.first, keyAndVal.second);
	}

	bool hitTable[3];
	for (int i = 0; i < 3; ++i) {
		hitTable[i] = false;
	}

	for (auto itr = map.CBegin(); itr != map.CEnd(); ++itr) {
		for (int i = 0; i < 3; ++i) {
			if (itr->first == keysAndVals[i].first && itr->second == keysAndVals[i].second) {
				hitTable[i] = true;
				break;
			}
		}
	}

	for (int i = 0; i < 3; ++i) {
		EXPECT_TRUE(hitTable[i]);
	}
}

static bool useMapTestObjDestructor = true;
static int mapTestObjCnt = 0;

class HashMapTestObj
{
public:
	HashMapTestObj()
	{
		++mapTestObjCnt;
	}

	HashMapTestObj(int v)
		: val(v)
	{
		++mapTestObjCnt;
	}

	~HashMapTestObj()
	{
		if (!useMapTestObjDestructor) {
			return;
		}
		--mapTestObjCnt;
	}

	int val = 0;
};

TEST(HashMap, InsertMove) {
	mapTestObjCnt = 0;
	useMapTestObjDestructor = false;

	BIEngine::HashMap<int, HashMapTestObj> map;

	HashMapTestObj obj;
	map.Insert(10, std::move(obj));
	EXPECT_EQ(mapTestObjCnt, 1);
}

TEST(HashMap, Emplace) {
	mapTestObjCnt = 0;
	useMapTestObjDestructor = true;

	{
		BIEngine::HashMap<int, HashMapTestObj> map;

		const std::pair<int, int> keysAndVals[] = { {10, 2}, {100, 8}, {25, 11} };

		for (const auto& keyAndVal : keysAndVals) {
			map.Emplace(keyAndVal.first, keyAndVal.second);
		}

		EXPECT_EQ(mapTestObjCnt, std::size(keysAndVals));

		bool hitTable[3];
		for (int i = 0; i < 3; ++i) {
			hitTable[i] = false;
		}

		for (auto itr = map.Begin(); itr != map.End(); ++itr) {
			for (int i = 0; i < 3; ++i) {
				if (itr->first == keysAndVals[i].first && itr->second.val == keysAndVals[i].second) {
					hitTable[i] = true;
					break;
				}
			}
		}

		for (int i = 0; i < 3; ++i) {
			EXPECT_TRUE(hitTable[i]);
		}
	}

	EXPECT_EQ(mapTestObjCnt, 0);
}

TEST(HashMap, EraseWithKey) {
	BIEngine::HashMap<int, int> map;

	const std::pair<int, int> keysAndVals[] = { {10, 2}, {100, 8}, {25, 11} };
	constexpr int idxKeyToDelete = 1;

	for (const auto& keyAndVal : keysAndVals) {
		map.Insert(keyAndVal.first, keyAndVal.second);
	}

	map.Erase(keysAndVals[idxKeyToDelete].first);

	bool hitTable[3];
	for (int i = 0; i < 3; ++i) {
		hitTable[i] = false;
	}

	for (auto itr = map.Begin(); itr != map.End(); ++itr) {
		for (int i = 0; i < 3; ++i) {
			if (itr->first == keysAndVals[i].first && itr->second == keysAndVals[i].second) {
				hitTable[i] = true;
				break;
			}
		}
	}

	for (int i = 0; i < 3; ++i) {
		if (i == idxKeyToDelete) {
			EXPECT_FALSE(hitTable[i]);
			continue;
		}

		EXPECT_TRUE(hitTable[i]);
	}
}

TEST(HashMap, EraseWithItr) {
	BIEngine::HashMap<int, int> map;

	const std::pair<int, int> keysAndVals[] = { {10, 2}, {100, 8}, {25, 11} };
	constexpr int idxKeyToDelete = 1;

	for (const auto& keyAndVal : keysAndVals) {
		map.Insert(keyAndVal.first, keyAndVal.second);
	}

	for (auto itr = map.Begin(); itr != map.End();) {
		if (itr->first == keysAndVals[idxKeyToDelete].first && itr->second == keysAndVals[idxKeyToDelete].second) {
			itr = map.Erase(itr);
			continue;
		}

		++itr;
	}

	bool hitTable[3];
	for (int i = 0; i < 3; ++i) {
		hitTable[i] = false;
	}

	for (auto itr = map.Begin(); itr != map.End(); ++itr) {
		for (int i = 0; i < 3; ++i) {
			if (itr->first == keysAndVals[i].first && itr->second == keysAndVals[i].second) {
				hitTable[i] = true;
				break;
			}
		}
	}

	for (int i = 0; i < 3; ++i) {
		if (i == idxKeyToDelete) {
			EXPECT_FALSE(hitTable[i]);
			continue;
		}

		EXPECT_TRUE(hitTable[i]);
	}
}


TEST(HashMap, InsertViaBracketOperator) {
	BIEngine::HashMap<int, int> map;

	const std::pair<int, int> keysAndVals[] = { {10, 2}, {100, 8}, {25, 11} };

	for (const auto& keyAndVal : keysAndVals) {
		map[keyAndVal.first] = keyAndVal.second;
	}

	bool hitTable[3];
	for (int i = 0; i < 3; ++i) {
		hitTable[i] = false;
	}

	for (auto itr = map.Begin(); itr != map.End(); ++itr) {
		for (int i = 0; i < 3; ++i) {
			if (itr->first == keysAndVals[i].first && itr->second == keysAndVals[i].second) {
				hitTable[i] = true;
				break;
			}
		}
	}

	for (int i = 0; i < 3; ++i) {
		EXPECT_TRUE(hitTable[i]);
	}
}

TEST(HashMap, Rehashing) {
	BIEngine::HashMap<int, int> map;

	const std::pair<int, int> keysAndVals[] = { {10, 2}, {102, 8}, {25, 11}, {24, 8}, {111, 25}, {32521, 86}, {100, 254}, {37, 12}, {1000, 24} };

	bool wasRehashed = false;
	float prevLoadFactor = 0.0f;
	for (const auto& keyAndVal : keysAndVals) {
		map.Insert(keyAndVal.first, keyAndVal.second);
		if (prevLoadFactor > map.LoadFactor()) {
			wasRehashed = true;
		}

		prevLoadFactor = map.LoadFactor();
	}

	EXPECT_EQ(map.Size(), std::size(keysAndVals));
	EXPECT_TRUE(wasRehashed);

	bool hitTable[std::size(keysAndVals)];
	for (int i = 0; i < std::size(hitTable); ++i) {
		hitTable[i] = false;
	}

	for (auto itr = map.Begin(); itr != map.End(); ++itr) {
		for (int i = 0; i < std::size(hitTable); ++i) {
			if (itr->first == keysAndVals[i].first && itr->second == keysAndVals[i].second) {
				hitTable[i] = true;
				break;
			}
		}
	}

	for (int i = 0; i < std::size(hitTable); ++i) {
		EXPECT_TRUE(hitTable[i]);
	}
}

TEST(HashMap, Find) {
	BIEngine::HashMap<int, int> map;

	std::pair<const int, int> keysAndVals[] = { {10, 2}, {100, 8}, {25, 11} };
	constexpr int idxKeyToFindAndChange = 1;
	constexpr int newVal = 2222;

	for (const auto& keyAndVal : keysAndVals) {
		map.Emplace(keyAndVal.first, keyAndVal.second);
	}

	auto findItr = map.Find(keysAndVals[idxKeyToFindAndChange].first);
	EXPECT_NE(findItr, map.End());

	findItr->second = newVal;
	keysAndVals[idxKeyToFindAndChange].second = newVal;

	bool hitTable[3];
	for (int i = 0; i < 3; ++i) {
		hitTable[i] = false;
	}

	for (auto itr = map.Begin(); itr != map.End(); ++itr) {
		for (int i = 0; i < 3; ++i) {
			if (itr->first == keysAndVals[i].first && itr->second == keysAndVals[i].second) {
				hitTable[i] = true;
				break;
			}
		}
	}

	for (int i = 0; i < 3; ++i) {
		EXPECT_TRUE(hitTable[i]);
	}
}
