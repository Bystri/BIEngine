#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

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
   BoneAnimChannel(const std::string boneName, const std::vector<KeyPosition>& positions, const std::vector<KeyRotation>& rotations, const std::vector<KeyScale>& scales)
      : m_boneName(boneName), m_localTransform(1.0f),
        m_positionCurve(constructPositionCurve(positions)),
        m_rotationCurve(constructRotationCurve(rotations)),
        m_scaleCurve(constructScaleCurve(scales))
   {
      m_positionFramesTimes.push_back(0.0f);
      for (int i = 0; i < positions.size(); ++i) {
         m_positionFramesTimes.push_back(positions[i].timeStamp);
      }

      m_rotationFramesTimes.push_back(0.0f);
      for (int i = 0; i < rotations.size(); ++i) {
         m_rotationFramesTimes.push_back(rotations[i].timeStamp);
      }

      m_scaleFramesTimes.push_back(0.0f);
      for (int i = 0; i < scales.size(); ++i) {
         m_scaleFramesTimes.push_back(scales[i].timeStamp);
      }
   }

   void Update(float animationTime);

   glm::mat4 GetLocalTransform() { return m_localTransform; }

   const std::string& GetBoneName() const { return m_boneName; }

private:
   static CatmullRomSpline constructPositionCurve(const std::vector<KeyPosition>& positions);
   static CatmullRomSpline4d constructRotationCurve(const std::vector<KeyRotation>& rotations);
   static CatmullRomSpline constructScaleCurve(const std::vector<KeyScale>& scales);

private:
   CatmullRomSpline m_positionCurve;
   CatmullRomSpline4d m_rotationCurve;
   CatmullRomSpline m_scaleCurve;

   std::vector<float> m_positionFramesTimes;
   std::vector<float> m_rotationFramesTimes;
   std::vector<float> m_scaleFramesTimes;

   glm::mat4 m_localTransform;
   std::string m_boneName;
};

} // namespace BIEngine
