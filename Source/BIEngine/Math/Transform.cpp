#include "Transform.h"

#include <cmath>

namespace BIEngine {

Transform::Transform()
   : mat()
{
   for (int i = 0; i < 4; ++i) {
      mat[i][i] = 1.0f;
   }
}

Transform& Transform::Translate(const Vector3& transVec)
{
   Transform translateTransform;
   translateTransform.mat[3] = Vector4(transVec, 1.0f);

   mat = translateTransform.mat * mat;

   return *this;
}

Transform& Transform::RotateX(const float rad)
{
   Transform rotateTrans;
   Matrix4& rotMat = rotateTrans.mat;

   rotMat[1][1] = std::cos(rad);
   rotMat[1][2] = std::sin(rad);
   rotMat[2][1] = -std::sin(rad);
   rotMat[2][2] = std::cos(rad);

   mat = rotMat * mat;

   return *this;
}

Transform& Transform::RotateY(const float rad)
{
   Transform rotateTrans;
   Matrix4& rotMat = rotateTrans.mat;

   rotMat[0][0] = std::cos(rad);
   rotMat[0][2] = -std::sin(rad);
   rotMat[2][0] = std::sin(rad);
   rotMat[2][2] = std::cos(rad);

   mat = rotMat * mat;

   return *this;
}

Transform& Transform::RotateZ(const float rad)
{
   Transform rotateTrans;
   Matrix4& rotMat = rotateTrans.mat;

   rotMat[0][0] = std::cos(rad);
   rotMat[0][1] = std::sin(rad);
   rotMat[1][0] = -std::sin(rad);
   rotMat[1][1] = std::cos(rad);

   mat = rotMat * mat;

   return *this;
}

Transform& Transform::RotateAroundAxis(const float rad, const Vector3& axis)
{
   const Vector3 w = Normalized(axis);

   Vector3 t = w;
   const float ux = std::abs(t.x);
   const float uy = std::abs(t.y);
   const float uz = std::abs(t.z);
   if (ux <= uy && ux <= uz) {
      t.x = 1.0f;
   } else if (uy <= ux && uy <= uz) {
      t.y = 1.0f;
   } else {
      t.z = 1.0f;
   }

   const Vector3 u = Normalized(Cross(t, w));
   const Vector3 v = Normalized(Cross(w, u));


   Transform axisChangeTransform;
   axisChangeTransform.mat[0][0] = u[0];
   axisChangeTransform.mat[1][0] = u[1];
   axisChangeTransform.mat[2][0] = u[2];
   axisChangeTransform.mat[0][1] = v[0];
   axisChangeTransform.mat[1][1] = v[1];
   axisChangeTransform.mat[2][1] = v[2];
   axisChangeTransform.mat[0][2] = w[0];
   axisChangeTransform.mat[1][2] = w[1];
   axisChangeTransform.mat[2][2] = w[2];

   mat = axisChangeTransform.mat * mat;

   RotateZ(rad);

   axisChangeTransform.mat[0] = Vector4(u, 0.0f);
   axisChangeTransform.mat[1] = Vector4(v, 0.0f);
   axisChangeTransform.mat[2] = Vector4(w, 0.0f);
   mat = axisChangeTransform.mat * mat;

   return *this;
}

Transform& Transform::Scale(const Vector3& scale)
{
   Transform scaleTranform;
   scaleTranform.mat[0][0] = scale[0];
   scaleTranform.mat[1][1] = scale[1];
   scaleTranform.mat[2][2] = scale[2];

   mat = scaleTranform.mat * mat;

   return *this;
}

Transform& Transform::ApplyTransform(const Transform& trans)
{
   mat = trans.mat * mat;
   return *this;
}

Vector3 Transform::GetTranslation() const
{
   return Vector3(mat[3].x, mat[3].y, mat[3].z);
}

Vector3 Transform::GetScale() const
{
   return Vector3(Length(mat[0]), Length(mat[1]), Length(mat[2]));
}

void Transform::TransformDirection(Vector3& dir) const
{
   Vector4 res = mat * Vector4(dir, 0.0f);
   dir.x = res.x;
   dir.y = res.y;
   dir.z = res.z;
}

void Transform::TransformLocation(Vector3& location) const
{
   Vector4 res = mat * Vector4(location, 1.0f);
   location.x = res.x;
   location.y = res.y;
   location.z = res.z;
}

const Transform Transform::GetInvTransformation() const
{
   const Vector3 invTranslation = GetTranslation() * -1.0f;

   Vector3 invScale = GetScale();
   invScale = Vector3(1.0f / invScale.x, 1.0f / invScale.y, 1.0f / invScale.z);

   Transform invRotTransform = *this;
   invRotTransform.mat[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
   invRotTransform.Scale(invScale);
   Transpose(invRotTransform.mat);

   Transform ret;
   ret.Translate(invTranslation);
   ret.ApplyTransform(invRotTransform);
   ret.Scale(invScale);

   return ret;
}

} // namespace BIEngine
