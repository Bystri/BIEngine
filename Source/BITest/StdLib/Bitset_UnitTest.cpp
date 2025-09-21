#include "gtest/gtest.h"

#include "../../BIEngine/StdLib/Bitset.h"

TEST(Bitset, Test) {
	{
		const char* strBits = "000000000000000000";
		BIEngine::Bitset<18> bitset(strBits);

		for (int i = 0; i < 18; ++i) {
			EXPECT_EQ(bitset.Test(17 - i), strBits[i] == '1');
		}
	}

	{
		const char* strBits = "010000000100000010";
		BIEngine::Bitset<18> bitset(strBits);

		for (int i = 0; i < 18; ++i) {
			EXPECT_EQ(bitset.Test(17 - i), strBits[i] == '1');
		}
	}

	{
		const char* strBits = "111111111111111111";
		BIEngine::Bitset<18> bitset(strBits);

		for (int i = 0; i < 18; ++i) {
			EXPECT_EQ(bitset.Test(17 - i), strBits[i] == '1');
		}
	}
}

TEST(Bitset, BracketOperator) {
	{
		const char* strBits = "001010000001000100";
		BIEngine::Bitset<18> bitset(strBits);

		EXPECT_EQ(bitset.Count(), 4);

		for (int i = 0; i < 18; ++i) {
			EXPECT_EQ(bitset[17-i], strBits[i] == '1');
		}

		for (int i = 0; i < 18; ++i) {
			bitset[i] = i % 2 == 0;
		}

		EXPECT_EQ(bitset.Count(), 9);

		for (int i = 0; i < 18; ++i) {
			EXPECT_EQ(bitset.Test(i), i % 2 == 0);
		}
	}
}

TEST(Bitset, All) {
	{
		BIEngine::Bitset<18> bitset("000000000000000000");

		EXPECT_FALSE(bitset.All());
	}

	{
		BIEngine::Bitset<18> bitset("000000000100000000");

		EXPECT_FALSE(bitset.All());
	}

	{
		BIEngine::Bitset<18> bitset("111111111111111111");

		EXPECT_TRUE(bitset.All());
	}
}

TEST(Bitset, Any) {
	{
		BIEngine::Bitset<18> bitset("000000000000000000");

		EXPECT_FALSE(bitset.Any());
	}

	{
		BIEngine::Bitset<18> bitset("000000000100000000");

		EXPECT_TRUE(bitset.Any());
	}

	{
		BIEngine::Bitset<18> bitset("111111111111111111");

		EXPECT_TRUE(bitset.Any());
	}
}


TEST(Bitset, None) {
	{
		BIEngine::Bitset<18> bitset("000000000000000000");

		EXPECT_TRUE(bitset.None());
	}

	{
		BIEngine::Bitset<18> bitset("000000000100000000");

		EXPECT_FALSE(bitset.None());
	}

	{
		BIEngine::Bitset<18> bitset("111111111111111111");

		EXPECT_FALSE(bitset.None());
	}
}

TEST(Bitset, Counter) {
	{
		BIEngine::Bitset<18> bitset("000000000000000000");

		EXPECT_EQ(bitset.Count(), 0);
	}

	{
		BIEngine::Bitset<18> bitset("000000000100000000");

		EXPECT_EQ(bitset.Count(), 1);
	}

	{
		BIEngine::Bitset<18> bitset("111111111111111111");

		EXPECT_EQ(bitset.Count(), 18);
	}
}

TEST(Bitset, UllCtor) {
	{
		BIEngine::Bitset<70> bitset(ULLONG_MAX);

		for (int i = 0; i < 64; ++i) {
			EXPECT_TRUE(bitset.Test(i));
		}

		for (int i = 64; i < 70; ++i) {
			EXPECT_FALSE(bitset.Test(i));
		}
	}

	{
		BIEngine::Bitset<8> bitset(42);

		EXPECT_FALSE(bitset.Test(0));
		EXPECT_TRUE(bitset.Test(1));
		EXPECT_FALSE(bitset.Test(2));
		EXPECT_TRUE(bitset.Test(3));
		EXPECT_FALSE(bitset.Test(4));
		EXPECT_TRUE(bitset.Test(5));
		EXPECT_FALSE(bitset.Test(6));
		EXPECT_FALSE(bitset.Test(7));
	}
}

TEST(Bitset, StringContainerCtor) {
	{
		BIEngine::String bitString = "110010";
		BIEngine::Bitset<8> bitset(bitString);

		EXPECT_FALSE(bitset.Test(0));
		EXPECT_TRUE(bitset.Test(1));
		EXPECT_FALSE(bitset.Test(2));
		EXPECT_FALSE(bitset.Test(3));
		EXPECT_TRUE(bitset.Test(4));
		EXPECT_TRUE(bitset.Test(5));
		EXPECT_FALSE(bitset.Test(6));
		EXPECT_FALSE(bitset.Test(7));
	}
}

TEST(Bitset, ComparasionOperators) {
	{
		BIEngine::String bitString = "00101010";
		BIEngine::Bitset<8> bitset1(bitString);

		BIEngine::Bitset<8> bitset2(42);

		EXPECT_TRUE(bitset1 == bitset2);
		EXPECT_FALSE(bitset1 != bitset2);
	}

	{
		BIEngine::String bitString = "11101010";
		BIEngine::Bitset<8> bitset1(bitString);

		BIEngine::Bitset<8> bitset2(42);

		EXPECT_FALSE(bitset1 == bitset2);
		EXPECT_TRUE(bitset1 != bitset2);
	}
}

TEST(Bitset, Set) {
	{
		BIEngine::Bitset<18> bitset("000000000000000000");

		EXPECT_EQ(bitset.Count(), 0);

		bitset.Set();

		EXPECT_EQ(bitset.Count(), 18);
	}
}

TEST(Bitset, SetWithIdx) {
	{
		BIEngine::Bitset<18> bitset("000000000000000000");

		EXPECT_EQ(bitset.Count(), 0);

		constexpr int idxsToSetTrue[3] = { 5, 8, 16 };
		
		for (int i = 0; i < 3; ++i) {
			bitset.Set(idxsToSetTrue[i]);
		}

		EXPECT_EQ(bitset.Count(), 3);

		for (int i = 0; i < 3; ++i) {
			EXPECT_TRUE(bitset.Test(idxsToSetTrue[i]), 3);
		}
	}

	{
		BIEngine::Bitset<18> bitset("111111111111111111");

		EXPECT_EQ(bitset.Count(), 18);

		constexpr int idxsToSetFalse[3] = { 5, 8, 16 };

		for (int i = 0; i < 3; ++i) {
			bitset.Set(idxsToSetFalse[i], false);
		}

		EXPECT_EQ(bitset.Count(), 15);

		for (int i = 0; i < 3; ++i) {
			EXPECT_FALSE(bitset.Test(idxsToSetFalse[i]), 3);
		}
	}
}

TEST(Bitset, Reset) {
	{
		BIEngine::Bitset<18> bitset("000000000000000000");

		EXPECT_EQ(bitset.Count(), 0);

		bitset.Reset();

		EXPECT_EQ(bitset.Count(), 0);
	}

	{
		BIEngine::Bitset<18> bitset("111111111111111111");

		EXPECT_EQ(bitset.Count(), 18);

		bitset.Reset();

		EXPECT_EQ(bitset.Count(), 0);
	}
}

TEST(Bitset, ResetWithIdx) {
	{
		BIEngine::Bitset<18> bitset("000000000000000000");

		EXPECT_EQ(bitset.Count(), 0);

		constexpr int idxsToSetFalse[3] = { 5, 8, 16 };

		for (int i = 0; i < 3; ++i) {
			bitset.Reset(idxsToSetFalse[i]);
		}

		EXPECT_EQ(bitset.Count(), 0);

		for (int i = 0; i < 3; ++i) {
			EXPECT_FALSE(bitset.Test(idxsToSetFalse[i]));
		}
	}


	{
		BIEngine::Bitset<18> bitset("111111111111111111");

		EXPECT_EQ(bitset.Count(), 18);

		constexpr int idxsToSetFalse[3] = { 5, 8, 16 };

		for (int i = 0; i < 3; ++i) {
			bitset.Reset(idxsToSetFalse[i]);
		}

		EXPECT_EQ(bitset.Count(), 15);

		for (int i = 0; i < 3; ++i) {
			EXPECT_FALSE(bitset.Test(idxsToSetFalse[i]));
		}
	}
}

TEST(Bitset, Flip) {
	{
		BIEngine::Bitset<18> bitset("000000000000000000");

		EXPECT_EQ(bitset.Count(), 0);

		bitset.Flip();

		EXPECT_EQ(bitset.Count(), 18);

		for (int i = 0; i < 18; ++i) {
			EXPECT_TRUE(bitset.Test(i));
		}
	}


	{
		BIEngine::Bitset<18> bitset("111111111111111111");

		EXPECT_EQ(bitset.Count(), 18);

		bitset.Flip();

		EXPECT_EQ(bitset.Count(), 0);

		for (int i = 0; i < 18; ++i) {
			EXPECT_FALSE(bitset.Test(i));
		}
	}
}

TEST(Bitset, FlipWithIdx) {
	{
		BIEngine::Bitset<18> bitset("000000000000000000");

		EXPECT_EQ(bitset.Count(), 0);

		constexpr int idxsToFlip[3] = { 5, 8, 16 };

		for (int i = 0; i < 3; ++i) {
			bitset.Flip(idxsToFlip[i]);
		}

		EXPECT_EQ(bitset.Count(), 3);

		for (int i = 0; i < 3; ++i) {
			EXPECT_TRUE(bitset.Test(idxsToFlip[i]));
		}
	}


	{
		BIEngine::Bitset<18> bitset("111111111111111111");

		EXPECT_EQ(bitset.Count(), 18);

		constexpr int idxsToFlip[3] = { 5, 8, 16 };

		for (int i = 0; i < 3; ++i) {
			bitset.Flip(idxsToFlip[i]);
		}

		EXPECT_EQ(bitset.Count(), 15);

		for (int i = 0; i < 3; ++i) {
			EXPECT_FALSE(bitset.Test(idxsToFlip[i]));
		}
	}
}

TEST(Bitset, MemberOperatorAnd) {
	const char* bitsetStr1 = "000111100010101010";
	const char* bitsetStr2 = "011110000001010101";

	{
		BIEngine::Bitset<18> bitset1(bitsetStr1);
		BIEngine::Bitset<18> bitset2(bitsetStr2);

		bitset1 &= bitset2;
		EXPECT_EQ(bitset1.Count(), 2);

		EXPECT_FALSE(bitset1.Test(0));
		EXPECT_FALSE(bitset1.Test(1));
		EXPECT_FALSE(bitset1.Test(2));
		EXPECT_FALSE(bitset1.Test(3));
		EXPECT_FALSE(bitset1.Test(4));
		EXPECT_FALSE(bitset1.Test(5));
		EXPECT_FALSE(bitset1.Test(6));
		EXPECT_FALSE(bitset1.Test(7));
		EXPECT_FALSE(bitset1.Test(8));
		EXPECT_FALSE(bitset1.Test(9));
		EXPECT_FALSE(bitset1.Test(10));
		EXPECT_FALSE(bitset1.Test(11));
		EXPECT_FALSE(bitset1.Test(12));
		EXPECT_TRUE(bitset1.Test(13));
		EXPECT_TRUE(bitset1.Test(14));
		EXPECT_FALSE(bitset1.Test(15));
		EXPECT_FALSE(bitset1.Test(16));
		EXPECT_FALSE(bitset1.Test(17));
	}
}

TEST(Bitset, MemberOperatorOr) {
	const char* bitsetStr1 = "000111100010101010";
	const char* bitsetStr2 = "011110000001010101";

	{
		BIEngine::Bitset<18> bitset1(bitsetStr1);
		BIEngine::Bitset<18> bitset2(bitsetStr2);

		bitset1 |= bitset2;
		EXPECT_EQ(bitset1.Count(), 14);

		EXPECT_TRUE(bitset1.Test(0));
		EXPECT_TRUE(bitset1.Test(1));
		EXPECT_TRUE(bitset1.Test(2));
		EXPECT_TRUE(bitset1.Test(3));
		EXPECT_TRUE(bitset1.Test(4));
		EXPECT_TRUE(bitset1.Test(5));
		EXPECT_TRUE(bitset1.Test(6));
		EXPECT_TRUE(bitset1.Test(7));
		EXPECT_FALSE(bitset1.Test(8));
		EXPECT_FALSE(bitset1.Test(9));
		EXPECT_FALSE(bitset1.Test(10));
		EXPECT_TRUE(bitset1.Test(11));
		EXPECT_TRUE(bitset1.Test(12));
		EXPECT_TRUE(bitset1.Test(13));
		EXPECT_TRUE(bitset1.Test(14));
		EXPECT_TRUE(bitset1.Test(15));
		EXPECT_TRUE(bitset1.Test(16));
		EXPECT_FALSE(bitset1.Test(17));
	}
}

TEST(Bitset, MemberOperatorXor) {
	const char* bitsetStr1 = "000111100010101010";
	const char* bitsetStr2 = "011110000001010101";

	{
		BIEngine::Bitset<18> bitset1(bitsetStr1);
		BIEngine::Bitset<18> bitset2(bitsetStr2);

		bitset1 ^= bitset2;
		EXPECT_EQ(bitset1.Count(), 12);

		EXPECT_TRUE(bitset1.Test(0));
		EXPECT_TRUE(bitset1.Test(1));
		EXPECT_TRUE(bitset1.Test(2));
		EXPECT_TRUE(bitset1.Test(3));
		EXPECT_TRUE(bitset1.Test(4));
		EXPECT_TRUE(bitset1.Test(5));
		EXPECT_TRUE(bitset1.Test(6));
		EXPECT_TRUE(bitset1.Test(7));
		EXPECT_FALSE(bitset1.Test(8));
		EXPECT_FALSE(bitset1.Test(9));
		EXPECT_FALSE(bitset1.Test(10));
		EXPECT_TRUE(bitset1.Test(11));
		EXPECT_TRUE(bitset1.Test(12));
		EXPECT_FALSE(bitset1.Test(13));
		EXPECT_FALSE(bitset1.Test(14));
		EXPECT_TRUE(bitset1.Test(15));
		EXPECT_TRUE(bitset1.Test(16));
		EXPECT_FALSE(bitset1.Test(17));
	}
}

TEST(Bitset, MemberOperatorBitsFlip) {
	const char* bitsetStr1 = "000111100010101010";

	{
		BIEngine::Bitset<18> bitset1(bitsetStr1);

		bitset1 = ~bitset1;
		EXPECT_EQ(bitset1.Count(), 10);

		EXPECT_TRUE(bitset1.Test(0));
		EXPECT_FALSE(bitset1.Test(1));
		EXPECT_TRUE(bitset1.Test(2));
		EXPECT_FALSE(bitset1.Test(3));
		EXPECT_TRUE(bitset1.Test(4));
		EXPECT_FALSE(bitset1.Test(5));
		EXPECT_TRUE(bitset1.Test(6));
		EXPECT_FALSE(bitset1.Test(7));
		EXPECT_TRUE(bitset1.Test(8));
		EXPECT_TRUE(bitset1.Test(9));
		EXPECT_TRUE(bitset1.Test(10));
		EXPECT_FALSE(bitset1.Test(11));
		EXPECT_FALSE(bitset1.Test(12));
		EXPECT_FALSE(bitset1.Test(13));
		EXPECT_FALSE(bitset1.Test(14));
		EXPECT_TRUE(bitset1.Test(15));
		EXPECT_TRUE(bitset1.Test(16));
		EXPECT_TRUE(bitset1.Test(17));
	}
}

TEST(Bitset, MemberGlobalAnd) {
	const char* bitsetStr1 = "000111100010101010";
	const char* bitsetStr2 = "011110000001010101";

	{
		BIEngine::Bitset<18> bitset1(bitsetStr1);
		BIEngine::Bitset<18> bitset2(bitsetStr2);

		BIEngine::Bitset<18> bitset3 = bitset1 & bitset2;
		EXPECT_EQ(bitset3.Count(), 2);

		EXPECT_FALSE(bitset3.Test(0));
		EXPECT_FALSE(bitset3.Test(1));
		EXPECT_FALSE(bitset3.Test(2));
		EXPECT_FALSE(bitset3.Test(3));
		EXPECT_FALSE(bitset3.Test(4));
		EXPECT_FALSE(bitset3.Test(5));
		EXPECT_FALSE(bitset3.Test(6));
		EXPECT_FALSE(bitset3.Test(7));
		EXPECT_FALSE(bitset3.Test(8));
		EXPECT_FALSE(bitset3.Test(9));
		EXPECT_FALSE(bitset3.Test(10));
		EXPECT_FALSE(bitset3.Test(11));
		EXPECT_FALSE(bitset3.Test(12));
		EXPECT_TRUE(bitset3.Test(13));
		EXPECT_TRUE(bitset3.Test(14));
		EXPECT_FALSE(bitset3.Test(15));
		EXPECT_FALSE(bitset3.Test(16));
		EXPECT_FALSE(bitset3.Test(17));
	}
}

TEST(Bitset, GlobalOperatorOr) {
	const char* bitsetStr1 = "000111100010101010";
	const char* bitsetStr2 = "011110000001010101";

	{
		BIEngine::Bitset<18> bitset1(bitsetStr1);
		BIEngine::Bitset<18> bitset2(bitsetStr2);

		BIEngine::Bitset<18> bitset3 = bitset1 | bitset2;
		EXPECT_EQ(bitset3.Count(), 14);

		EXPECT_TRUE(bitset3.Test(0));
		EXPECT_TRUE(bitset3.Test(1));
		EXPECT_TRUE(bitset3.Test(2));
		EXPECT_TRUE(bitset3.Test(3));
		EXPECT_TRUE(bitset3.Test(4));
		EXPECT_TRUE(bitset3.Test(5));
		EXPECT_TRUE(bitset3.Test(6));
		EXPECT_TRUE(bitset3.Test(7));
		EXPECT_FALSE(bitset3.Test(8));
		EXPECT_FALSE(bitset3.Test(9));
		EXPECT_FALSE(bitset3.Test(10));
		EXPECT_TRUE(bitset3.Test(11));
		EXPECT_TRUE(bitset3.Test(12));
		EXPECT_TRUE(bitset3.Test(13));
		EXPECT_TRUE(bitset3.Test(14));
		EXPECT_TRUE(bitset3.Test(15));
		EXPECT_TRUE(bitset3.Test(16));
		EXPECT_FALSE(bitset3.Test(17));
	}
}

TEST(Bitset, GlobalOperatorXor) {
	const char* bitsetStr1 = "000111100010101010";
	const char* bitsetStr2 = "011110000001010101";

	{
		BIEngine::Bitset<18> bitset1(bitsetStr1);
		BIEngine::Bitset<18> bitset2(bitsetStr2);

		BIEngine::Bitset<18> bitset3 = bitset1 ^ bitset2;
		EXPECT_EQ(bitset3.Count(), 12);

		EXPECT_TRUE(bitset3.Test(0));
		EXPECT_TRUE(bitset3.Test(1));
		EXPECT_TRUE(bitset3.Test(2));
		EXPECT_TRUE(bitset3.Test(3));
		EXPECT_TRUE(bitset3.Test(4));
		EXPECT_TRUE(bitset3.Test(5));
		EXPECT_TRUE(bitset3.Test(6));
		EXPECT_TRUE(bitset3.Test(7));
		EXPECT_FALSE(bitset3.Test(8));
		EXPECT_FALSE(bitset3.Test(9));
		EXPECT_FALSE(bitset3.Test(10));
		EXPECT_TRUE(bitset3.Test(11));
		EXPECT_TRUE(bitset3.Test(12));
		EXPECT_FALSE(bitset3.Test(13));
		EXPECT_FALSE(bitset3.Test(14));
		EXPECT_TRUE(bitset3.Test(15));
		EXPECT_TRUE(bitset3.Test(16));
		EXPECT_FALSE(bitset3.Test(17));
	}
}