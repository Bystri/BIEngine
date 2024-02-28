#include "gtest/gtest.h"

#include "../../BIEngine/Math/Trigonometry.h"

const float EPS = 0.001f;

TEST(Trigonometry, RadToDeg) {
	const float rad = 1.309;
	const float deg = BIEngine::RadToDeg(rad);

	EXPECT_NEAR(deg, 75.0f, EPS);
}

TEST(Trigonometry, DegToRad) {
	const float deg = 75.0f;
	const float rad = BIEngine::DegToRad(deg);

	EXPECT_NEAR(rad, 1.309f, EPS);
}

TEST(Trigonometry, RadToDegDouble) {
	const double rad = 1.309;
	const double deg = BIEngine::RadToDeg(rad);

	EXPECT_NEAR(deg, 75.0, EPS);
}

TEST(Trigonometry, DegToRadDouble) {
	const double deg = 75.0;
	const double rad = BIEngine::DegToRad(deg);

	EXPECT_NEAR(rad, 1.309, EPS);
}