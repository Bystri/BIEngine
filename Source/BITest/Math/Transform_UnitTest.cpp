#include "gtest/gtest.h"

#include "../../BIEngine/Math/Math.h"
#include "../../BIEngine/Math/Transform.h"
#include "../../BIEngine/Math/Trigonometry.h"

const float EPS = 0.001f;

TEST(Transform, ApplyIdentityToDirection) {
	BIEngine::Transform transform;

	BIEngine::Vector3 dir = {2.0f, 3.0f, 1.0f};

	transform.TransformDirection(dir);

	EXPECT_FLOAT_EQ(dir.x, 2.0f);
	EXPECT_FLOAT_EQ(dir.y, 3.0f);
	EXPECT_FLOAT_EQ(dir.z, 1.0f);
}

TEST(Transform, ApplyIdentityToLocation) {
	BIEngine::Transform transform;

	BIEngine::Vector3 loc = { 2.0f, 3.0f, 1.0f };

	transform.TransformLocation(loc);

	EXPECT_FLOAT_EQ(loc.x, 2.0f);
	EXPECT_FLOAT_EQ(loc.y, 3.0f);
	EXPECT_FLOAT_EQ(loc.z, 1.0f);
}

TEST(Transform, ApplyTranslateionToDirection) {
	BIEngine::Transform transform;

	const BIEngine::Vector3 translation = { 1.0f, -5.0f, 0.0f };
	BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };

	transform.Translate(translation).TransformDirection(dir);

	EXPECT_FLOAT_EQ(dir.x, 2.0f);
	EXPECT_FLOAT_EQ(dir.y, 3.0f);
	EXPECT_FLOAT_EQ(dir.z, 1.0f);
}

TEST(Transform, ApplyTranslateionToLocation) {
	BIEngine::Transform transform;

	const BIEngine::Vector3 translation = { 1.0f, -5.0f, 0.0f };
	BIEngine::Vector3 loc = { 2.0f, 3.0f, 1.0f };

	transform.Translate(translation).TransformLocation(loc);

	EXPECT_FLOAT_EQ(loc.x, 3.0f);
	EXPECT_FLOAT_EQ(loc.y, -2.0f);
	EXPECT_FLOAT_EQ(loc.z, 1.0f);
}

TEST(Transform, ApplyScaleToDirection) {
	BIEngine::Transform transform;

	const BIEngine::Vector3 scale = { 1.0f, -2.0f, 2.0f };
	BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };

	transform.Scale(scale).TransformDirection(dir);

	EXPECT_FLOAT_EQ(dir.x, 2.0f);
	EXPECT_FLOAT_EQ(dir.y, -6.0f);
	EXPECT_FLOAT_EQ(dir.z, 2.0f);
}

TEST(Transform, ApplyScaleToLocation) {
	BIEngine::Transform transform;

	const BIEngine::Vector3 scale = { 1.0f, -2.0f, 2.0f };
	BIEngine::Vector3 loc = { 2.0f, 3.0f, 1.0f };

	transform.Scale(scale).TransformLocation(loc);

	EXPECT_FLOAT_EQ(loc.x, 2.0f);
	EXPECT_FLOAT_EQ(loc.y, -6.0f);
	EXPECT_FLOAT_EQ(loc.z, 2.0f);
}

TEST(Transform, ApplyRotationXToDirection) {
	BIEngine::Transform transform;

	const float rad = BIEngine::DegToRad(75.0f);
	BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };

	transform.RotateX(rad).TransformDirection(dir);

	EXPECT_NEAR(dir.x, 2.0f, EPS);
	EXPECT_NEAR(dir.y, -0.18947f, EPS);
	EXPECT_NEAR(dir.z, 3.1566f, EPS);
}

TEST(Transform, ApplyRotationXToLocation) {
	BIEngine::Transform transform;

	const float rad = BIEngine::DegToRad(75.0f);
	BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };

	transform.RotateX(rad).TransformLocation(dir);

	EXPECT_NEAR(dir.x, 2.0f, EPS);
	EXPECT_NEAR(dir.y, -0.18947f, EPS);
	EXPECT_NEAR(dir.z, 3.1566f, EPS);
}

TEST(Transform, ApplyRotationYToDirection) {
	BIEngine::Transform transform;

	const float rad = BIEngine::DegToRad(75.0f);
	BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };

	transform.RotateY(rad).TransformDirection(dir);

	EXPECT_NEAR(dir.x, 1.48356f, EPS);
	EXPECT_NEAR(dir.y, 3.0f, EPS);
	EXPECT_NEAR(dir.z, -1.67303f, EPS);
}

TEST(Transform, ApplyRotationYToLocation) {
	BIEngine::Transform transform;

	const float rad = BIEngine::DegToRad(75.0f);
	BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };

	transform.RotateY(rad).TransformLocation(dir);

	EXPECT_NEAR(dir.x, 1.48356f, EPS);
	EXPECT_NEAR(dir.y, 3.0f, EPS);
	EXPECT_NEAR(dir.z, -1.67303f, EPS);
}

TEST(Transform, ApplyRotationZToDirection) {
	BIEngine::Transform transform;

	const float rad = BIEngine::DegToRad(75.0f);
	BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };

	transform.RotateZ(rad).TransformDirection(dir);

	EXPECT_NEAR(dir.x, -2.38014f, EPS);
	EXPECT_NEAR(dir.y, 2.70831f, EPS);
	EXPECT_NEAR(dir.z, 1.0f, EPS);
}

TEST(Transform, ApplyRotationZToLocation) {
	BIEngine::Transform transform;

	const float rad = BIEngine::DegToRad(75.0f);
	BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };

	transform.RotateZ(rad).TransformLocation(dir);

	EXPECT_NEAR(dir.x, -2.38014f, EPS);
	EXPECT_NEAR(dir.y, 2.70831f, EPS);
	EXPECT_NEAR(dir.z, 1.0f, EPS);
}

TEST(Transform, ApplyRotationAroundAxisToDirection) {
	const float rad = BIEngine::DegToRad(75.0f);

	{
		BIEngine::Transform transformX;
		BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };
		const BIEngine::Vector3 axisX = { 1.0f, 0.0f, 0.0f };

		transformX.RotateAroundAxis(rad, axisX).TransformDirection(dir);

		EXPECT_NEAR(dir.x, 2.0f, EPS);
		EXPECT_NEAR(dir.y, -0.18947f, EPS);
		EXPECT_NEAR(dir.z, 3.1566f, EPS);
	}
	{
		BIEngine::Transform transformY;
		BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };
		const BIEngine::Vector3 axisY = { 0.0f, 1.0f, 0.0f };

		transformY.RotateAroundAxis(rad, axisY).TransformDirection(dir);

		EXPECT_NEAR(dir.x, 1.48356f, EPS);
		EXPECT_NEAR(dir.y, 3.0f, EPS);
		EXPECT_NEAR(dir.z, -1.67303f, EPS);
	}
	{
		BIEngine::Transform transformY;
		BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };
		const BIEngine::Vector3 axisY = { 0.0f, 0.0f, 1.0f };
		transformY.RotateAroundAxis(rad, axisY).TransformDirection(dir);

		EXPECT_NEAR(dir.x, -2.38014f, EPS);
		EXPECT_NEAR(dir.y, 2.70831f, EPS);
		EXPECT_NEAR(dir.z, 1.0f, EPS);
	}
	{
		BIEngine::Transform transformArb;

		BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };
		const BIEngine::Vector3 axisArb = { 4.0f, 2.0f, 3.0f };
		transformArb.RotateAroundAxis(rad, axisArb).TransformDirection(dir);

		EXPECT_NEAR(dir.x, 1.0f, EPS);
		EXPECT_NEAR(dir.y, 2.00416f, EPS);
		EXPECT_NEAR(dir.z, 2.99722f, EPS);
	}
}

TEST(Transform, ApplyRotationAroundAxisToLocation) {
	const float rad = BIEngine::DegToRad(75.0f);

	{
		BIEngine::Transform transformX;
		BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };
		const BIEngine::Vector3 axisX = { 1.0f, 0.0f, 0.0f };

		transformX.RotateAroundAxis(rad, axisX).TransformLocation(dir);

		EXPECT_NEAR(dir.x, 2.0f, EPS);
		EXPECT_NEAR(dir.y, -0.18947f, EPS);
		EXPECT_NEAR(dir.z, 3.1566f, EPS);
	}
	{
		BIEngine::Transform transformY;
		BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };
		const BIEngine::Vector3 axisY = { 0.0f, 1.0f, 0.0f };

		transformY.RotateAroundAxis(rad, axisY).TransformLocation(dir);

		EXPECT_NEAR(dir.x, 1.48356f, EPS);
		EXPECT_NEAR(dir.y, 3.0f, EPS);
		EXPECT_NEAR(dir.z, -1.67303f, EPS);
	}
	{
		BIEngine::Transform transformY;
		BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };
		const BIEngine::Vector3 axisY = { 0.0f, 0.0f, 1.0f };
		transformY.RotateAroundAxis(rad, axisY).TransformLocation(dir);

		EXPECT_NEAR(dir.x, -2.38014f, EPS);
		EXPECT_NEAR(dir.y, 2.70831f, EPS);
		EXPECT_NEAR(dir.z, 1.0f, EPS);
	}
	{
		BIEngine::Transform transformArb;

		BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };
		const BIEngine::Vector3 axisArb = { 4.0f, 2.0f, 3.0f };
		transformArb.RotateAroundAxis(rad, axisArb).TransformLocation(dir);

		EXPECT_NEAR(dir.x, 1.0f, EPS);
		EXPECT_NEAR(dir.y, 2.00416f, EPS);
		EXPECT_NEAR(dir.z, 2.99722f, EPS);
	}
}

TEST(Transform, ApplyScaleTranslateToDirection) {
	BIEngine::Transform transform;

	const BIEngine::Vector3 translation = { 1.0f, -5.0f, 0.0f };
	const BIEngine::Vector3 scale = { 1.0f, -2.0f, 2.0f };
	BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };

	transform.Scale(scale).Translate(translation).TransformDirection(dir);

	EXPECT_FLOAT_EQ(dir.x, 2.0f);
	EXPECT_FLOAT_EQ(dir.y, -6.0f);
	EXPECT_FLOAT_EQ(dir.z, 2.0f);
}

TEST(Transform, ApplyScaleTranslateToLocation) {
	BIEngine::Transform transform;

	const BIEngine::Vector3 translation = { 1.0f, -5.0f, 0.0f };
	const BIEngine::Vector3 scale = { 1.0f, -2.0f, 2.0f };
	BIEngine::Vector3 loc = { 2.0f, 3.0f, 1.0f };

	transform.Scale(scale).Translate(translation).TransformLocation(loc);

	EXPECT_FLOAT_EQ(loc.x, 3.0f);
	EXPECT_FLOAT_EQ(loc.y, -11.0f);
	EXPECT_FLOAT_EQ(loc.z, 2.0f);
}

TEST(Transform, ApplyScaledTranslationToDirection) {
	BIEngine::Transform transform;

	const BIEngine::Vector3 translation = { 1.0f, -5.0f, 0.0f };
	const BIEngine::Vector3 scale = { 1.0f, -2.0f, 2.0f };
	BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };

	transform.Translate(translation).Scale(scale).TransformDirection(dir);

	EXPECT_FLOAT_EQ(dir.x, 2.0f);
	EXPECT_FLOAT_EQ(dir.y, -6.0f);
	EXPECT_FLOAT_EQ(dir.z, 2.0f);
}

TEST(Transform, ApplyScaledTranslationToLocation) {
	BIEngine::Transform transform;

	const BIEngine::Vector3 translation = { 1.0f, -5.0f, 0.0f };
	const BIEngine::Vector3 scale = { 1.0f, -2.0f, 2.0f };
	BIEngine::Vector3 loc = { 2.0f, 3.0f, 1.0f };

	transform.Translate(translation).Scale(scale).TransformLocation(loc);

	EXPECT_FLOAT_EQ(loc.x, 3.0f);
	EXPECT_FLOAT_EQ(loc.y, 4.0f);
	EXPECT_FLOAT_EQ(loc.z, 2.0f);
}

TEST(Transform, ApplyScaleRotationTranslationToDirection) {
	const float rad = BIEngine::DegToRad(75.0f);
	
	BIEngine::Transform transform;

	const BIEngine::Vector3 translation = { 1.0f, -5.0f, 0.0f };
	const BIEngine::Vector3 scale = { 1.0f, -2.0f, 2.0f };
	BIEngine::Vector3 dir = { 2.0f, 3.0f, 1.0f };

	transform.Scale(scale).RotateX(rad).Translate(translation).TransformDirection(dir);

	EXPECT_NEAR(dir.x, 2.0f, EPS);
	EXPECT_NEAR(dir.y, -3.48477f, EPS);
	EXPECT_NEAR(dir.z, -5.27792f, EPS);
}

TEST(Transform, ApplyScaleRotationTranslationToLocation) {
	const float rad = BIEngine::DegToRad(75.0f);

	BIEngine::Transform transform;

	const BIEngine::Vector3 translation = { 1.0f, -5.0f, 0.0f };
	const BIEngine::Vector3 scale = { 1.0f, -2.0f, 2.0f };
	BIEngine::Vector3 loc = { 2.0f, 3.0f, 1.0f };

	transform.Scale(scale).RotateX(rad).Translate(translation).TransformLocation(loc);

	EXPECT_NEAR(loc.x, 3.0f, EPS);
	EXPECT_NEAR(loc.y, -8.48477f, EPS);
	EXPECT_NEAR(loc.z, -5.27792f, EPS);
}

TEST(Transform, InverseScaleRotationTranslation) {
	const float rad = BIEngine::DegToRad(75.0f);

	BIEngine::Transform transform;

	const BIEngine::Vector3 translation = { 1.0f, -5.0f, 0.0f };
	const BIEngine::Vector3 scale = { 1.0f, -2.0f, 2.0f };
	BIEngine::Vector3 loc = { 2.0f, 3.0f, 1.0f };

	transform.Scale(scale).RotateX(rad).Translate(translation).TransformLocation(loc);
	const BIEngine::Transform invTrans = transform.GetInvTransformation();
	invTrans.TransformLocation(loc);

	EXPECT_NEAR(loc.x, 2.0f, EPS);
	EXPECT_NEAR(loc.y, 3.0f, EPS);
	EXPECT_NEAR(loc.z, 1.0f, EPS);
}

TEST(Transform, InverseScaledTranslatedRotation) {
	const float rad = BIEngine::DegToRad(75.0f);

	BIEngine::Transform transform;

	const BIEngine::Vector3 translation = { 1.0f, -5.0f, 0.0f };
	const BIEngine::Vector3 scale = { 1.0f, -2.0f, 2.0f };
	BIEngine::Vector3 loc = { 2.0f, 3.0f, 1.0f };

	transform.Translate(translation).RotateX(rad).Scale(scale).TransformLocation(loc);
	const BIEngine::Transform invTrans = transform.GetInvTransformation();
	invTrans.TransformLocation(loc);

	EXPECT_FLOAT_EQ(loc.x, 2.0f);
	EXPECT_FLOAT_EQ(loc.y, 3.0f);
	EXPECT_FLOAT_EQ(loc.z, 1.0f);
}

TEST(Transform, CameraTransform) {
	const BIEngine::Vector3 eyePos = BIEngine::Vector3(2.0f, 1.0f, 0.0f);
	const BIEngine::Vector3 gazeDir = BIEngine::Vector3(1.0f, -1.0f, 0.0f);
	const BIEngine::Vector3 camUp = BIEngine::Vector3(1.0f, 1.0f, 0.0f);
	BIEngine::Transform camTrans = BIEngine::CalcCameraTrans(eyePos, gazeDir, camUp);

	BIEngine::Vector3 worldPoint = BIEngine::Vector3(3.0f, 0.0f, 0.0f);

	camTrans.TransformLocation(worldPoint);

	EXPECT_FLOAT_EQ(worldPoint.x, 0.0f);
	EXPECT_FLOAT_EQ(worldPoint.y, 0.0f);
	EXPECT_FLOAT_EQ(worldPoint.z, -1.41421342f);
}

TEST(Transform, OrthographicProjection) {
	BIEngine::Vector3 lbn = BIEngine::Vector3(-2.0f, -3.0f, -4.0f);
	BIEngine::Vector3 rtf = BIEngine::Vector3(4.0f, 0.0f, -8.0f);

	BIEngine::Vector3 midPoint = rtf + (lbn - rtf) * 0.5f;

	BIEngine::Transform orthoProj = BIEngine::CalcOrthoProj(lbn.x, rtf.x, lbn.y, rtf.y, lbn.z, rtf.z);

	orthoProj.TransformLocation(lbn);

	EXPECT_FLOAT_EQ(lbn.x, -1.0f);
	EXPECT_FLOAT_EQ(lbn.y, -1.0f);
	EXPECT_FLOAT_EQ(lbn.z, 1.0f);

	orthoProj.TransformLocation(rtf);

	EXPECT_FLOAT_EQ(rtf.x, 1.0f);
	EXPECT_FLOAT_EQ(rtf.y, 1.0f);
	EXPECT_FLOAT_EQ(rtf.z, -1.0f);

	orthoProj.TransformLocation(midPoint);

	EXPECT_FLOAT_EQ(midPoint.x, 0.0f);
	EXPECT_FLOAT_EQ(midPoint.y, 0.0f);
	EXPECT_FLOAT_EQ(midPoint.z, 0.0f);
}

TEST(Transform, PerspectiveProjection) {
	const float fovRad = BIEngine::DegToRad(45.0f);
	const float aspectRatio = 1024.0f / 768.0f;
	const float near = -4.0f;
	const float far = -8.0f;

	const float t = BIEngine::Abs(near) * BIEngine::Tan(fovRad / 2.0f);
	const float r = t * aspectRatio;

	const float t2 = BIEngine::Abs(far) * BIEngine::Tan(fovRad / 2.0f);
	const float r2 = t2 * aspectRatio;

	BIEngine::Transform perspectiveProj = BIEngine::CalcPerspectiveProj(fovRad, aspectRatio, near, far);

	BIEngine::Vector3 lbn = BIEngine::Vector3(-r, -t, near);
	BIEngine::Vector3 rtn = BIEngine::Vector3(r, t, near);

	BIEngine::Vector3 lbn2 = BIEngine::Vector3(-r2, -t2, far);
	BIEngine::Vector3 rtn2 = BIEngine::Vector3(r2, t2, far);

	perspectiveProj.TransformLocation(lbn);

	EXPECT_FLOAT_EQ(lbn.x, -1.0f);
	EXPECT_FLOAT_EQ(lbn.y, -1.0f);
	EXPECT_FLOAT_EQ(lbn.z, 1.0f);

	perspectiveProj.TransformLocation(rtn);

	EXPECT_FLOAT_EQ(rtn.x, 1.0f);
	EXPECT_FLOAT_EQ(rtn.y, 1.0f);
	EXPECT_FLOAT_EQ(rtn.z, 1.0f);

	perspectiveProj.TransformLocation(lbn2);

	EXPECT_FLOAT_EQ(lbn2.x, -1.0f);
	EXPECT_FLOAT_EQ(lbn2.y, -1.0f);
	EXPECT_FLOAT_EQ(lbn2.z, -1.0f);

	perspectiveProj.TransformLocation(rtn2);

	EXPECT_FLOAT_EQ(rtn2.x, 1.0f);
	EXPECT_FLOAT_EQ(rtn2.y, 1.0f);
	EXPECT_FLOAT_EQ(rtn2.z, -1.0f);
}