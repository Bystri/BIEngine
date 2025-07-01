#pragma once

#include <string>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

#include "../StdLib/DynamicArray.h"
#include "../EngineCore/Assert.h"
#include "../Math/Spline.h"

namespace BIEngine {

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
   BoneAnimChannel(const std::string boneName, const DynamicArray<KeyPosition>& positions, const DynamicArray<KeyRotation>& rotations, const DynamicArray<KeyScale>& scales)
      : m_boneName(boneName), m_localTransform(1.0f),
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

   void Update(float animationTime);

   glm::mat4 GetLocalTransform() { return m_localTransform; }

   const std::string& GetBoneName() const { return m_boneName; }

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

   glm::mat4 m_localTransform;
   std::string m_boneName;
};

} // namespace BIEngine
