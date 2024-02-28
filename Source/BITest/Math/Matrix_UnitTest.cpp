#include "gtest/gtest.h"

#include "../../BIEngine/Math/Matrix.h"

TEST(Matrix, AddTwoMatrices) {
	BIEngine::Matrix<float, 2, 3> mat1({ {1.0f, 2.0f}, {3.0f, 4.0f}, {5.0f, 6.0f} });
	BIEngine::Matrix<float, 2, 3> mat2({ {-10.0f, 20.0f}, {30.0f, -40.0f}, {-50.0f, -60.0f} });

	mat1 = mat1 + mat2;

	//first column
	EXPECT_FLOAT_EQ(mat1[0].x, -9.0f);
	EXPECT_FLOAT_EQ(mat1[0].y, 22.0f);

	//second column
	EXPECT_FLOAT_EQ(mat1[1].x, 33.0f);
	EXPECT_FLOAT_EQ(mat1[1].y, -36.0f);

	//third column
	EXPECT_FLOAT_EQ(mat1[2].x, -45.0f);
	EXPECT_FLOAT_EQ(mat1[2].y, -54.0f);
}

TEST(Matrix, SubtructTwoMatrices) {
	BIEngine::Matrix<float, 2, 3> mat1({ {1.0f, 2.0f}, {3.0f, 4.0f}, {5.0f, 6.0f} });
	BIEngine::Matrix<float, 2, 3> mat2({ {-10.0f, 20.0f}, {30.0f, -40.0f}, {-50.0f, -60.0f} });

	mat1 = mat1 - mat2;

	//first column
	EXPECT_FLOAT_EQ(mat1[0].x, 11.0f);
	EXPECT_FLOAT_EQ(mat1[0].y, -18.0f);

	//second column
	EXPECT_FLOAT_EQ(mat1[1].x, -27.0f);
	EXPECT_FLOAT_EQ(mat1[1].y, 44.0f);

	//third column
	EXPECT_FLOAT_EQ(mat1[2].x, 55.0f);
	EXPECT_FLOAT_EQ(mat1[2].y, 66.0f);
}

TEST(Matrix, MultMatrixScalar) {
	BIEngine::Matrix2 mat1({ {1.0f, 2.0f}, {3.0f, 4.0f} });
	const float scalar = 3.0f;

	mat1 = mat1 * scalar;

	//first column
	EXPECT_FLOAT_EQ(mat1[0].x, 3.0f);
	EXPECT_FLOAT_EQ(mat1[0].y, 6.0f);

	//second column
	EXPECT_FLOAT_EQ(mat1[1].x, 9.0f);
	EXPECT_FLOAT_EQ(mat1[1].y, 12.0f);

}

TEST(Matrix, MultMatrixVector) {
	BIEngine::Matrix<float, 3, 2> mat1({ {0.0f, -1.0f, 2.0f}, {1.0f, 0.0f, 4.0f} });
	BIEngine::Vector2 vec1({4.0f, 10.0f});

	const BIEngine::Vector3 vec2 = mat1 * vec1;

	//first column
	EXPECT_FLOAT_EQ(vec2.x, 10.0f);
	EXPECT_FLOAT_EQ(vec2.y, -4.0f);
	EXPECT_FLOAT_EQ(vec2.z, 48.0f);
}


TEST(Matrix, MultMatrixMatrix) {
    BIEngine::Matrix<float, 3, 2> mat1({ {0.0f, 2.0f, 4.0f}, {1.0f, 3.0f, 5.0f} });
	BIEngine::Matrix<float, 2, 4> mat2({ {6.0f, 0.0f}, {7.0f, 1.0f}, {8.0f, 2.0f}, {9.0f, 3.0f} });

	BIEngine::Matrix<float, 3, 4> mat3 = mat1 * mat2;

	//first column
	EXPECT_FLOAT_EQ(mat3[0].x, 0.0f);
	EXPECT_FLOAT_EQ(mat3[0].y, 12.0f);
	EXPECT_FLOAT_EQ(mat3[0].z, 24.0f);

	//second column
	EXPECT_FLOAT_EQ(mat3[1].x, 1.0f);
	EXPECT_FLOAT_EQ(mat3[1].y, 17.0f);
	EXPECT_FLOAT_EQ(mat3[1].z, 33.0f);

	//third column
	EXPECT_FLOAT_EQ(mat3[2].x, 2.0f);
	EXPECT_FLOAT_EQ(mat3[2].y, 22.0f);
	EXPECT_FLOAT_EQ(mat3[2].z, 42.0f);

	//fourth column
	EXPECT_FLOAT_EQ(mat3[3].x, 3.0f);
	EXPECT_FLOAT_EQ(mat3[3].y, 27.0f);
	EXPECT_FLOAT_EQ(mat3[3].z, 51.0f);
}

TEST(Matrix, TransposeMatrix) {
	BIEngine::Matrix<float, 3, 2> mat1({ {1.0f, 3.0f, 5.0f}, {2.0f, 4.0f, 6.0f} });

	BIEngine::Matrix<float, 2, 3> res = Transposed(mat1);

	//first column
	EXPECT_FLOAT_EQ(res[0].x, 1.0f);
	EXPECT_FLOAT_EQ(res[0].y, 2.0f);

	//second column
	EXPECT_FLOAT_EQ(res[1].x, 3.0f);
	EXPECT_FLOAT_EQ(res[1].y, 4.0f);

	//third column
	EXPECT_FLOAT_EQ(res[2].x, 5.0f);
	EXPECT_FLOAT_EQ(res[2].y, 6.0f);
}

TEST(Matrix, DeterminantMatrix) {
	BIEngine::Matrix<float, 2, 2> mat1({ {3.0f, 4.0f}, {8.0f, 6.0f} });

	const float res1 = Determinant(mat1);

	EXPECT_FLOAT_EQ(res1, -14.0f);

	BIEngine::Matrix<float, 3, 3> mat2({ {6.0f, 4.0f, 2.0f}, {1.0f, -2.0f, 8.0f}, {1.0f, 5.0f, 7.0f} });

	const float res2 = Determinant(mat2);

	EXPECT_FLOAT_EQ(res2, -306.0f);

	BIEngine::Matrix<float, 4, 4> mat3({ {1.0f, 3.0f, 0.0f, 0.0f}, {3.0f, 9.0f, 2.0f, 4.0f}, {1.0f, 5.0f, 1.0f, 2.0f}, {4.0f, 15.0f, 1.0f, 3.0f} });

	const float res3 = Determinant(mat3);

	EXPECT_FLOAT_EQ(res3, -4.0f);
}

TEST(Matrix, InverseMatrix) {
	BIEngine::Matrix<float, 2, 2> mat1({ {4.0f, 3.0f}, {3.0f, 2.0f} });

	const BIEngine::Matrix<float, 2, 2> inverse1 = Inverse(mat1);

	//first column
	EXPECT_FLOAT_EQ(inverse1[0][0], -2.0f);
	EXPECT_FLOAT_EQ(inverse1[0][1], 3.0f);

	//second column
	EXPECT_FLOAT_EQ(inverse1[1][0], 3.0f);
	EXPECT_FLOAT_EQ(inverse1[1][1], -4.0f);

	BIEngine::Matrix<float, 3, 3> mat2({ {1.0f, -1.0f, 1.0f}, {0.0f, 2.0f, 1.0f}, {1.0f, 2.0f, 2.0f} });

	const BIEngine::Matrix<float, 3, 3> inverse2 = Inverse(mat2);

	//first column
	EXPECT_FLOAT_EQ(inverse2[0][0], -2.0f);
	EXPECT_FLOAT_EQ(inverse2[0][1], -4.0f);
	EXPECT_FLOAT_EQ(inverse2[0][2], 3.0f);

	//second column
	EXPECT_FLOAT_EQ(inverse2[1][0], -1.0f);
	EXPECT_FLOAT_EQ(inverse2[1][1], -1.0f);
	EXPECT_FLOAT_EQ(inverse2[1][2], 1.0f);

	//third column
	EXPECT_FLOAT_EQ(inverse2[2][0], 2.0f);
	EXPECT_FLOAT_EQ(inverse2[2][1], 3.0f);
	EXPECT_FLOAT_EQ(inverse2[2][2], -2.0f);

	BIEngine::Matrix<float, 4, 4> mat3({ {1.0f, 0.0f, 2.0f, 2.0f}, {0.0f, 2.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 2.0f, 1.0f, 4.0f} });

	const BIEngine::Matrix<float, 4, 4> inverse3 = Inverse(mat3);

	//first column
	EXPECT_FLOAT_EQ(inverse3[0][0], -2.0f);
	EXPECT_FLOAT_EQ(inverse3[0][1], 1.0f);
	EXPECT_FLOAT_EQ(inverse3[0][2], -8.0f);
	EXPECT_FLOAT_EQ(inverse3[0][3], 3.0f);

	//second column
	EXPECT_FLOAT_EQ(inverse3[1][0], -0.5f);
	EXPECT_FLOAT_EQ(inverse3[1][1], 0.5f);
	EXPECT_FLOAT_EQ(inverse3[1][2], -1.0f);
	EXPECT_FLOAT_EQ(inverse3[1][3], 0.5f);

	//third column
	EXPECT_FLOAT_EQ(inverse3[2][0], 1.0f);
	EXPECT_FLOAT_EQ(inverse3[2][1], 0.0f);
	EXPECT_FLOAT_EQ(inverse3[2][2], 2.0f);
	EXPECT_FLOAT_EQ(inverse3[2][3], -1.0f);

	//fourth column
	EXPECT_FLOAT_EQ(inverse3[3][0], 0.5f);
	EXPECT_FLOAT_EQ(inverse3[3][1], -0.5f);
	EXPECT_FLOAT_EQ(inverse3[3][2], 2.0f);
	EXPECT_FLOAT_EQ(inverse3[3][3], -0.5f);
}