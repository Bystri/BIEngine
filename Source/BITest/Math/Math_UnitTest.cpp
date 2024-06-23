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

TEST(Math, Min) {
	const float min = -10.0f;
	const float max = 100.0f;

	EXPECT_FLOAT_EQ(BIEngine::Min(min, max), min);
	EXPECT_FLOAT_EQ(BIEngine::Min(min, min), min);
}

TEST(Math, Max) {
	const float min = -10.0f;
	const float max = 100.0f;

	EXPECT_FLOAT_EQ(BIEngine::Max(min, max), max);
	EXPECT_FLOAT_EQ(BIEngine::Max(max, max), max);
}

TEST(Math, Clamp) {
	const float min = -10.0f;
	const float mid = 2.0f;
	const float max = 100.0f;

	EXPECT_FLOAT_EQ(BIEngine::Clamp(max, min, mid), mid);
	EXPECT_FLOAT_EQ(BIEngine::Clamp(min, mid, max), mid);
	EXPECT_FLOAT_EQ(BIEngine::Clamp(mid, min, max), mid);
	EXPECT_FLOAT_EQ(BIEngine::Clamp(mid, mid, mid), mid);
}

TEST(Math, SmoothDamp) {
	const float from = -10.0f;
	const float to = 100.0f;

	const float smoothTime = 0.1f;
	const float deltaTime = 1.0f / 60.0f;
	constexpr float maxSpeed = std::numeric_limits<float>::infinity();

	float currentVelocity = 0.0f;

	EXPECT_FLOAT_EQ(BIEngine::SmoothDamp(from, to, currentVelocity, smoothTime, deltaTime, maxSpeed), -5.1094970f);
	EXPECT_FLOAT_EQ(BIEngine::SmoothDamp(from, to, currentVelocity, smoothTime, deltaTime, maxSpeed), 1.1677856f);
	EXPECT_FLOAT_EQ(BIEngine::SmoothDamp(from, to, currentVelocity, smoothTime, deltaTime, maxSpeed), 4.1668853f);
}