#include "gtest/gtest.h"

#include "../../BIEngine/Math/Vector.h"

TEST(Vector, UnarMinusVector) {
	BIEngine::Vector2 vec1(1.0f, 2.0f);

	BIEngine::Vector2 vec3 = -vec1;

	EXPECT_FLOAT_EQ(vec3.x, -1.0f);
	EXPECT_FLOAT_EQ(vec3.y, -2.0f);
}

TEST(Vector, AddTwoVectors) {
	BIEngine::Vector2 vec1(1.0f, 2.0f);
	BIEngine::Vector2 vec2(3.0f, 4.0f);

	BIEngine::Vector2 vec3 = vec1 + vec2;

	EXPECT_FLOAT_EQ(vec3.x, 4.0f);
	EXPECT_FLOAT_EQ(vec3.y, 6.0f);
}

TEST(Vector, SubtractTwoVectors) {
	BIEngine::Vector2 vec1(1.0f, 4.0f);
	BIEngine::Vector2 vec2(3.0f, 2.0f);

	BIEngine::Vector2 vec3 = vec1 - vec2;

	EXPECT_FLOAT_EQ(vec3.x, -2.0f);
	EXPECT_FLOAT_EQ(vec3.y, 2.0f);
}

TEST(Vector, MultVectorScalar) {
	BIEngine::Vector2 vec1(1.0f, 4.0f);

	BIEngine::Vector2 vec2 = vec1 * 2.0f;

	EXPECT_FLOAT_EQ(vec2.x, 2.0f);
	EXPECT_FLOAT_EQ(vec2.y, 8.0f);
}

TEST(Vector, DivVectorScalar) {
	BIEngine::Vector2 vec1(1.0f, 4.0f);

	BIEngine::Vector2 vec2 = vec1 / 2.0f;

	EXPECT_FLOAT_EQ(vec2.x, 0.5f);
	EXPECT_FLOAT_EQ(vec2.y, 2.0f);
}

TEST(Vector, Length) {
	BIEngine::Vector2 vec1(4.0f, 3.0f);

	const float length2 = Length2(vec1);

	EXPECT_FLOAT_EQ(length2, 25.0f);

	const float length = Length(vec1);

	EXPECT_FLOAT_EQ(length, 5.0f);
}

TEST(Vector, DotProduct) {
	BIEngine::Vector2 vec1(1.0f, 4.0f);
	BIEngine::Vector2 vec2(3.0f, 2.0f);

	const float dot = Dot(vec1, vec2);

	EXPECT_FLOAT_EQ(dot, 11.0f);
}

TEST(Vector, CrossProduct) {
	BIEngine::Vector3 vec1(1.0f, 0.0f, 0.0f);
	BIEngine::Vector3 vec2(0.0f, 1.0f, 0.0f);

	const BIEngine::Vector3 cross = Cross(vec1, vec2);

	EXPECT_FLOAT_EQ(cross.x, 0.0f);
	EXPECT_FLOAT_EQ(cross.y, 0.0f);
	EXPECT_FLOAT_EQ(cross.z, 1.0f);
}