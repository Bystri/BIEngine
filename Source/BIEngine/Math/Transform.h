#pragma once

#include "Matrix.h"

namespace BIEngine {

class Transform {
public:
   Transform();

   Transform& Translate(const Vector3& transVec);

   Transform& RotateX(const float rad);
   Transform& RotateY(const float rad);
   Transform& RotateZ(const float rad);
   Transform& RotateAroundAxis(const float rad, const Vector3& axis);

   Transform& Scale(const Vector3& scale);

   Transform& ApplyTransform(const Transform& trans);

   Vector3 GetTranslation() const;
   Vector3 GetScale() const;

   void TransformDirection(Vector3& dir) const;
   void TransformLocation(Vector3& location) const;

   const Matrix4& GetMatrix() const { return mat; }

   const Transform GetInvTransformation() const;

private:
   Matrix4 mat;
};

} // namespace BIEngine
