#include "gtest/gtest.h"

#include "../../BIEngine/Math/Math.h"

TEST(Math, Abs) {
	const float posVal = 14.309f;
	const float negVal = -posVal;

	EXPECT_FLOAT_EQ(BIEngine::Abs(posVal), posVal);
	EXPECT_FLOAT_EQ(BIEngine::Abs(negVal), posVal);
}

TEST(Math, Sqrt) {
	const float val = 4.0f;

	EXPECT_FLOAT_EQ(BIEngine::Sqrt(val), 2.0f);
}