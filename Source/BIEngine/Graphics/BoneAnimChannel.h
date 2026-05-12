#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

#include "../StdLib/String.h"
#include "../StdLib/DynamicArray.h"
#include "../Math/Spline.h"

namespace BIEngine {

class BoneAnimTranformInfo {
   friend class BoneAnimChannel;

public:
   const glm::vec3& GetPos() const
   {
      return m_position;
   }

   const glm::quat GetOrientation() const
   {
      return m_orientation;
   }

   const glm::vec3 GetScale() const
   {
      return m_scale;
   }

private:
   BoneAnimTranformInfo(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
      : m_position(position), m_orientation(orientation), m_scale(scale)
   {
   }

private:
   glm::vec3 m_position;
   glm::quat m_orientation;
   glm::vec3 m_scale;
};

class BoneAnimChannel {
public:
   struct KeyPosition {
      glm::vec3 position;
      float timeStamp;
   };

   struct KeyRotation {
      glm::quat orientation;
      float timeStamp;
   };

   struct KeyScale {
      glm::vec3 scale;
      float timeStamp;
   };

public:
   BoneAnimChannel(const String& boneName, const DynamicArray<KeyPosition>& positions, const DynamicArray<KeyRotation>& rotations, const DynamicArray<KeyScale>& scales)
      : m_boneName(boneName),
        m_positionCurve(constructPositionCurve(positions)),
        m_rotationCurve(constructRotationCurve(rotations)),
        m_scaleCurve(constructScaleCurve(scales))
   {
      m_positionFramesTimes.Reserve(positions.Size() + 1);
      m_positionFramesTimes.PushBack(0.0f);
      for (int i = 0; i < positions.Size(); ++i) {
         m_positionFramesTimes.PushBack(positions[i].timeStamp);
      }

      m_rotationFramesTimes.Reserve(positions.Size() + 1);
      m_rotationFramesTimes.PushBack(0.0f);
      for (int i = 0; i < rotations.Size(); ++i) {
         m_rotationFramesTimes.PushBack(rotations[i].timeStamp);
      }

      m_scaleFramesTimes.Reserve(positions.Size() + 1);
      m_scaleFramesTimes.PushBack(0.0f);
      for (int i = 0; i < scales.Size(); ++i) {
         m_scaleFramesTimes.PushBack(scales[i].timeStamp);
      }
   }

   BoneAnimTranformInfo GetTransformAtTime(float animationTime);

   const String& GetBoneName() const { return m_boneName; }

private:
   static CatmullRomSpline constructPositionCurve(const DynamicArray<KeyPosition>& positions);
   static CatmullRomSpline4d constructRotationCurve(const DynamicArray<KeyRotation>& rotations);
   static CatmullRomSpline constructScaleCurve(const DynamicArray<KeyScale>& scales);

private:
   CatmullRomSpline m_positionCurve;
   CatmullRomSpline4d m_rotationCurve;
   CatmullRomSpline m_scaleCurve;

   DynamicArray<float> m_positionFramesTimes;
   DynamicArray<float> m_rotationFramesTimes;
   DynamicArray<float> m_scaleFramesTimes;

   String m_boneName;
};

} // namespace BIEngine
