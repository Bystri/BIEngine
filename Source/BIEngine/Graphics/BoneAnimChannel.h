#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

#include "../EngineCore/Assert.h"

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
        m_positions(positions), m_rotations(rotations), m_scales(scales)
   {
   }

   void Update(float animationTime);

   glm::mat4 GetLocalTransform() { return m_localTransform; }

   const std::string& GetBoneName() const { return m_boneName; }

private:
   int getPositionIndex(float animationTime);
   int getRotationIndex(float animationTime);
   int getScaleIndex(float animationTime);

   float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

   glm::mat4 interpolatePosition(float animationTime);
   glm::mat4 interpolateRotation(float animationTime);
   glm::mat4 interpolateScaling(float animationTime);

private:
   std::vector<KeyPosition> m_positions;
   std::vector<KeyRotation> m_rotations;
   std::vector<KeyScale> m_scales;

   glm::mat4 m_localTransform;
   std::string m_boneName;
};

} // namespace BIEngine
