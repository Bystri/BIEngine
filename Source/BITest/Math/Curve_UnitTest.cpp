#include "gtest/gtest.h"

#include "../../BIEngine/Math/Curve.h"


TEST(Curve, LinearSpline) {
	const BIEngine::Vector3 p1(3.0f, 2.0f, 1.0f);
	const BIEngine::Vector3 p2(6.0f, 5.0f, 1.0f);
	const BIEngine::Vector3 p3(11.0f, 0.0f, 1.0f);

	BIEngine::LinearSpline spline({ p1, p2, p3 });

	{
		const float T = 0.5f;
		const BIEngine::Vector3 curvePoint = spline.GetPoint(T);

		EXPECT_FLOAT_EQ(curvePoint.x, 4.5f);
		EXPECT_FLOAT_EQ(curvePoint.y, 3.5f);
		EXPECT_FLOAT_EQ(curvePoint.z, 1.0f);
	}

	{
		const float T = 1.5f;
		const BIEngine::Vector3 curvePoint = spline.GetPoint(T);

		EXPECT_FLOAT_EQ(curvePoint.x, 8.5f);
		EXPECT_FLOAT_EQ(curvePoint.y, 2.5f);
		EXPECT_FLOAT_EQ(curvePoint.z, 1.0f);
	}
}

TEST(Curve, HermiteSpline) {
	const BIEngine::Vector2 p1(0.0f, 0.0f);
	const BIEngine::Vector2 vel1(0.0f, 3.0f);
	const BIEngine::Vector2 p2(1.0f, 0.0f);
	const BIEngine::Vector2 vel2(0.0f, -3.0f);
	const BIEngine::Vector2 p3(2.0f, 0.0f);
	const BIEngine::Vector2 vel3(0.0f, 3.0f);

	BIEngine::HermiteSpline2d spline({ p1, p2, p3 }, {vel1, vel2, vel3});

	{
		const float T = 0.5f;
		const BIEngine::Vector2 curvePoint = spline.GetPoint(T);

		EXPECT_FLOAT_EQ(curvePoint.x, 0.5f);
		EXPECT_FLOAT_EQ(curvePoint.y, 0.75f);
	}

	{
		const float T = 1.5f;
		const BIEngine::Vector2 curvePoint = spline.GetPoint(T);

		EXPECT_FLOAT_EQ(curvePoint.x, 1.5f);
		EXPECT_FLOAT_EQ(curvePoint.y, -0.75f);
	}
}

TEST(Curve, CatmullRomSpline) {
	const BIEngine::Vector2 p1(0.0f, 0.0f);
	const BIEngine::Vector2 p2(0.0f, 10.0f);
	const BIEngine::Vector2 p3(10.0f, 10.0f);
	const BIEngine::Vector2 p4(10.0f, 0.0f);

	BIEngine::CatmullRomSpline2d spline({ p1, p2, p3, p4 });

	{
		const float T = 0.0f;
		const BIEngine::Vector2 curvePoint = spline.GetPoint(T);

		EXPECT_FLOAT_EQ(curvePoint.x, 0.0f);
		EXPECT_FLOAT_EQ(curvePoint.y, 0.0f);
	}

	{
		const float T = 0.5f;
		const BIEngine::Vector2 curvePoint = spline.GetPoint(T);

		EXPECT_FLOAT_EQ(curvePoint.x, -0.625000000f);
		EXPECT_FLOAT_EQ(curvePoint.y, 5.62500000f);
	}

	{
		const float T = 1.5f;
		const BIEngine::Vector2 curvePoint = spline.GetPoint(T);

		EXPECT_FLOAT_EQ(curvePoint.x, 5.0f);
		EXPECT_FLOAT_EQ(curvePoint.y, 11.25f);
	}

	{
		const float T = 2.999999f;
		const BIEngine::Vector2 curvePoint = spline.GetPoint(T);

		EXPECT_FLOAT_EQ(curvePoint.x, 10.0f);
		EXPECT_FLOAT_EQ(curvePoint.y, 9.53674316e-06f);
	}
}

TEST(Curve, BSpline) {
	const BIEngine::Vector2 p1(0.0f, 0.0f);
	const BIEngine::Vector2 p2(1.0f, 1.0f);
	const BIEngine::Vector2 p3(4.0f, 1.0f);
	const BIEngine::Vector2 p4(5.0f, 0.5f);
	const BIEngine::Vector2 p5(7.0f, 0.0f);

	BIEngine::BSpline2d spline({ p1, p2, p3, p4, p5 });

	{
		const float T = 0.5f;
		const BIEngine::Vector2 curvePoint = spline.GetPoint(T);

		EXPECT_FLOAT_EQ(curvePoint.x, 2.50000024f);
		EXPECT_FLOAT_EQ(curvePoint.y, 0.968750060f);
	}

	{
		const float T = 1.0f;
		const BIEngine::Vector2 curvePoint = spline.GetPoint(T);

		EXPECT_FLOAT_EQ(curvePoint.x, 3.66666698f);
		EXPECT_FLOAT_EQ(curvePoint.y, 0.916666687f);
	}

	{
		const float T = 1.5f;
		const BIEngine::Vector2 curvePoint = spline.GetPoint(T);

		EXPECT_FLOAT_EQ(curvePoint.x, 4.47916698f);
		EXPECT_FLOAT_EQ(curvePoint.y, 0.739583373f);
	}

	{
		const float T = 1.9f;
		const BIEngine::Vector2 curvePoint = spline.GetPoint(T);

		EXPECT_FLOAT_EQ(curvePoint.x, 5.02116680f);
		EXPECT_FLOAT_EQ(curvePoint.y, 0.549916744f);
	}
}

TEST(Curve, BezierSpline) {
	const BIEngine::Vector2 p1(0.0f, 0.0f);
	const BIEngine::Vector2 p2(0.0f, 1.0f);
	const BIEngine::Vector2 p3(1.0f, 1.0f);
	const BIEngine::Vector2 p4(1.0f, 0.0f);
	const BIEngine::Vector2 p5(1.0f, -1.0f);
	const BIEngine::Vector2 p6(2.0f, -1.0f);
	const BIEngine::Vector2 p7(2.0f, 0.0f);


	BIEngine::BezierSpline2d spline({ p1, p2, p3, p4, p5, p6, p7 });

	{
		const float T = 0.5f;
		const BIEngine::Vector2 curvePoint = spline.GetPoint(T);

		EXPECT_FLOAT_EQ(curvePoint.x, 0.5f);
		EXPECT_FLOAT_EQ(curvePoint.y, 0.75f);
	}

	{
		const float T = 1.5f;
		const BIEngine::Vector2 curvePoint = spline.GetPoint(T);

		EXPECT_FLOAT_EQ(curvePoint.x, 1.5f);
		EXPECT_FLOAT_EQ(curvePoint.y, -0.75f);
	}
}