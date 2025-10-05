#include "gtest/gtest.h"

#include <type_traits>

#include "../../BIEngine/StdLib/Utility.h"

TEST(Pair, DefaultCtor) {
	BIEngine::Pair<int, bool> pr;

	EXPECT_EQ(pr.first, 0);
	EXPECT_EQ(pr.second, false);
}

TEST(Pair, CtorByElements) {
	BIEngine::Pair<int, bool> pr(1, true);

	EXPECT_EQ(pr.first, 1);
	EXPECT_EQ(pr.second, true);
}

TEST(Pair, CtorByElementsOfAnotherTypes) {
	BIEngine::Pair<int, bool> pr(2.0f, 1);

	EXPECT_EQ(pr.first, 2);
	EXPECT_EQ(pr.second, true);
}

TEST(Pair, CopyCtor) {
	BIEngine::Pair<int, bool> pr(1, true);

	BIEngine::Pair<int, bool> p2(pr);

	EXPECT_EQ(p2.first, 1);
	EXPECT_EQ(p2.second, true);
}

TEST(Pair, MoveCtor) {
	BIEngine::Pair<int, bool> pr(1, true);

	BIEngine::Pair<int, bool> p2(std::move(pr));

	EXPECT_EQ(p2.first, 1);
	EXPECT_EQ(p2.second, true);
}

TEST(Pair, CopyCtorByElementsOfAnotherTypes) {
	BIEngine::Pair<float, int> pr(3.0f, 0);

	BIEngine::Pair<int, bool> p2(pr);

	EXPECT_EQ(p2.first, 3);
	EXPECT_EQ(p2.second, false);
}

TEST(Pair, MoveCtorByElementsOfAnotherTypes) {
	BIEngine::Pair<float, int> pr(3.0f, 0);

	BIEngine::Pair<int, bool> p2(std::move(pr));

	EXPECT_EQ(p2.first, 3);
	EXPECT_EQ(p2.second, false);
}

TEST(Pair, AssegnemtnOperator) {
	BIEngine::Pair<int, bool> pr(3, true);

	BIEngine::Pair<int, bool> p2;
	p2 = pr;

	EXPECT_EQ(p2.first, 3);
	EXPECT_EQ(p2.second, true);
}

TEST(Pair, AssegnemtnOperatorByElementsOfAnotherTypes) {
	BIEngine::Pair<float, int> pr(3.0f, 0);

	BIEngine::Pair<int, bool> p2;

	p2 = pr;

	EXPECT_EQ(p2.first, 3);
	EXPECT_EQ(p2.second, false);
}

TEST(Pair, MoveAssegnemtnOperator) {
	BIEngine::Pair<int, bool> pr(3, true);

	BIEngine::Pair<int, bool> p2;
	p2 = std::move(pr);

	EXPECT_EQ(p2.first, 3);
	EXPECT_EQ(p2.second, true);
}

TEST(Pair, MoveAssegnemtnOperatorByElementsOfAnotherTypes) {
	BIEngine::Pair<float, int> pr(3.0f, 0);

	BIEngine::Pair<int, bool> p2;

	p2 = std::move(pr);

	EXPECT_EQ(p2.first, 3);
	EXPECT_EQ(p2.second, false);
}

TEST(Pair, OperatorEq) {
	BIEngine::Pair<float, int> pr(3.0f, 0);
	BIEngine::Pair<int, bool> p2(3, false);
	BIEngine::Pair<int, bool> p3(4, true);

	EXPECT_TRUE(pr == p2);
	EXPECT_FALSE(pr == p3);
}

TEST(Pair, OperatorNotEq) {
	BIEngine::Pair<float, int> pr(3.0f, 0);
	BIEngine::Pair<int, bool> p2(3, false);
	BIEngine::Pair<int, bool> p3(4, true);

	EXPECT_FALSE(pr != p2);
	EXPECT_TRUE(pr != p3);
}

TEST(Pair, OperatorLower) {
	BIEngine::Pair<float, int> pr(3.0f, 0);
	BIEngine::Pair<int, bool> p2(3, true);
	BIEngine::Pair<int, bool> p3(4, true);

	EXPECT_TRUE(pr < p2);
	EXPECT_TRUE(pr < p3);
	EXPECT_FALSE(p2 < pr);
	EXPECT_FALSE(p3 < pr);
}

TEST(Pair, OperatorLE) {
	BIEngine::Pair<float, int> pr(3.0f, 0);
	BIEngine::Pair<int, bool> p2(3, true);
	BIEngine::Pair<int, bool> p3(4, true);

	EXPECT_TRUE(pr <= p2);
	EXPECT_TRUE(pr <= p3);
	EXPECT_FALSE(p2 <= pr);
	EXPECT_FALSE(p3 <= pr);
}

TEST(Pair, OperatorGE) {
	BIEngine::Pair<float, int> pr(3.0f, 0);
	BIEngine::Pair<int, bool> p2(3, true);
	BIEngine::Pair<int, bool> p3(4, true);

	EXPECT_FALSE(pr >= p2);
	EXPECT_FALSE(pr >= p3);
	EXPECT_TRUE(p2 >= pr);
	EXPECT_TRUE(p3 >= pr);
}

TEST(Pair, MakePair) {
	const int i = 10;
	const bool b = false;
	const auto pr = BIEngine::MakePair(i, b);

	const bool isFirstTypeSame = std::is_same_v<decltype(pr)::FirstType, int>;
	EXPECT_TRUE(isFirstTypeSame);

	const bool isSecondTypeSame = std::is_same_v<decltype(pr)::SecondType, bool>;
	EXPECT_TRUE(isSecondTypeSame);

	EXPECT_EQ(pr.first, 10);
	EXPECT_EQ(pr.second, false);
}

TEST(Pair, MakePairMove) {
	const auto pr = BIEngine::MakePair(10, false);

	const bool isFirstTypeSame = std::is_same_v<decltype(pr)::FirstType, decltype(10)>;
	EXPECT_TRUE(isFirstTypeSame);

	const bool isSecondTypeSame = std::is_same_v<decltype(pr)::SecondType, decltype(false)>;
	EXPECT_TRUE(isSecondTypeSame);

	EXPECT_EQ(pr.first, 10);
	EXPECT_EQ(pr.second, false);
}