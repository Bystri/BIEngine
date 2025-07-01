#include "BoneAnimChannel.h"

namespace BIEngine {

static float getCurveU(const DynamicArray<float>& framesTimes, const float animationTime)
{
   for (int i = 0; i < framesTimes.Size() - 1; ++i) {
      if (animationTime < framesTimes[i + 1]) {
         return static_cast<float>(i) + (animationTime - framesTimes[i]) / (framesTimes[i + 1] - framesTimes[i]);
      }
   }

   Assert(false, "There is no position in channel's bone for time %f", animationTime);
   return -1;
}

void BoneAnimChannel::Update(float animationTime)
{
   {
      const float u = getCurveU(m_positionFramesTimes, animationTime);
      const Vector3 val = m_positionCurve.GetPointByU(u);
      m_localTransform = glm::translate(glm::mat4(1.0f), glm::vec3(val.x, val.y, val.z));
   }

   {
      const float u = getCurveU(m_rotationFramesTimes, animationTime);
      const Vector4 val = m_rotationCurve.GetPointByU(u);
      m_localTransform *= glm::mat4(glm::normalize(glm::quat(val.x, val.y, val.z, val.w)));
   }

   {
      const float u = getCurveU(m_scaleFramesTimes, animationTime);
      const Vector3 val = m_scaleCurve.GetPointByU(u);
      m_localTransform *= glm::scale(glm::mat4(1.0f), glm::vec3(val.x, val.y, val.z));
   }
}

CatmullRomSpline BoneAnimChannel::constructPositionCurve(const DynamicArray<KeyPosition>& positions)
{
   DynamicArray<Vector3> vals;
   vals.Reserve(positions.Size() + 1);

   vals.EmplaceBack(positions[positions.Size() - 1].position.x, positions[positions.Size() - 1].position.y, positions[positions.Size() - 1].position.z);
   for (int i = 0; i < positions.Size(); ++i) {
      vals.EmplaceBack(positions[i].position.x, positions[i].position.y, positions[i].position.z);
   }

   return CatmullRomSpline(vals);
}

CatmullRomSpline4d BoneAnimChannel::constructRotationCurve(const DynamicArray<KeyRotation>& rotations)
{
   DynamicArray<Vector4> vals;
   vals.Reserve(rotations.Size() + 1);

   vals.EmplaceBack(rotations[rotations.Size() - 1].orientation.w, rotations[rotations.Size() - 1].orientation.x, rotations[rotations.Size() - 1].orientation.y, rotations[rotations.Size() - 1].orientation.z);
   for (int i = 0; i < rotations.Size(); ++i) {
      vals.EmplaceBack(rotations[i].orientation.w, rotations[i].orientation.x, rotations[i].orientation.y, rotations[i].orientation.z);
   }

   return CatmullRomSpline4d(vals);
}

CatmullRomSpline BoneAnimChannel::constructScaleCurve(const DynamicArray<KeyScale>& scales)
{
   DynamicArray<Vector3> vals;
   vals.Reserve(scales.Size() + 1);

   vals.EmplaceBack(scales[scales.Size() - 1].scale.x, scales[scales.Size() - 1].scale.y, scales[scales.Size() - 1].scale.z);
   for (int i = 0; i < scales.Size(); ++i) {
      vals.EmplaceBack(scales[i].scale.x, scales[i].scale.y, scales[i].scale.z);
   }

   return CatmullRomSpline(vals);
}

} // namespace BIEngine
