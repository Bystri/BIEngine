#include "BoneAnimChannel.h"

namespace BIEngine {

void BoneAnimChannel::Update(float animationTime)
{
   const glm::mat4 translation = interpolatePosition(animationTime);
   const glm::mat4 rotation = interpolateRotation(animationTime);
   const glm::mat4 scale = interpolateScaling(animationTime);
   m_localTransform = translation * rotation * scale;
}

int BoneAnimChannel::getPositionIndex(float animationTime)
{
   for (int i = 0; i < m_positions.size() - 1; ++i) {
      if (animationTime < m_positions[i + 1].timeStamp) {
         return i;
      }
   }

   Assert(false, "There is no position in channel's bone for time %f", animationTime);
   return -1;
}

int BoneAnimChannel::getRotationIndex(float animationTime)
{
   for (int i = 0; i < m_rotations.size() - 1; ++i) {
      if (animationTime < m_rotations[i + 1].timeStamp) {
         return i;
      }
   }

   Assert(false, "There is no rotation in channel's bone for time %f", animationTime);
   return -1;
}

int BoneAnimChannel::getScaleIndex(float animationTime)
{
   for (int i = 0; i < m_scales.size() - 1; ++i) {
      if (animationTime < m_scales[i + 1].timeStamp) {
         return i;
      }
   }

   Assert(false, "There is no scales in channel's bone for time %f", animationTime);
   return -1;
}

float BoneAnimChannel::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
   const float midWayLength = animationTime - lastTimeStamp;
   const float framesDiff = nextTimeStamp - lastTimeStamp;
   return midWayLength / framesDiff;
}

glm::mat4 BoneAnimChannel::interpolatePosition(float animationTime)
{
   if (m_positions.size() == 1) {
      return glm::translate(glm::mat4(1.0f), m_positions[0].position);
   }

   const int p0Index = getPositionIndex(animationTime);
   const int p1Index = p0Index + 1;

   const float scaleFactor = getScaleFactor(m_positions[p0Index].timeStamp, m_positions[p1Index].timeStamp, animationTime);
   const glm::vec3 finalPosition = glm::mix(m_positions[p0Index].position, m_positions[p1Index].position, scaleFactor);
   return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 BoneAnimChannel::interpolateRotation(float animationTime)
{
   if (m_rotations.size() == 1) {
      return glm::mat4(glm::normalize(m_rotations[0].orientation));
   }

   const int p0Index = getRotationIndex(animationTime);
   const int p1Index = p0Index + 1;
   const float scaleFactor = getScaleFactor(m_rotations[p0Index].timeStamp, m_rotations[p1Index].timeStamp, animationTime);
   const glm::quat finalRotation = glm::slerp(m_rotations[p0Index].orientation, m_rotations[p1Index].orientation, scaleFactor);

   return glm::mat4(glm::normalize(finalRotation));
}

glm::mat4 BoneAnimChannel::interpolateScaling(float animationTime)
{
   if (m_scales.size() == 1) {
      return glm::scale(glm::mat4(1.0f), m_scales[0].scale);
   }

   const int p0Index = getScaleIndex(animationTime);
   const int p1Index = p0Index + 1;
   const float scaleFactor = getScaleFactor(m_scales[p0Index].timeStamp, m_scales[p1Index].timeStamp, animationTime);
   const glm::vec3 finalScale = glm::mix(m_scales[p0Index].scale, m_scales[p1Index].scale, scaleFactor);
   return glm::scale(glm::mat4(1.0f), finalScale);
}

} // namespace BIEngine
