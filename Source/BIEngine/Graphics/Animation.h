#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "BoneAnimChannel.h"

namespace BIEngine {

class Animation {
public:
   Animation(float duration, int ticksPerSecond, const std::vector<BoneAnimChannel>& boneChannels)
      : m_duration(duration), m_ticksPerSecond(ticksPerSecond), m_boneChannels(boneChannels)
   {
   }

   BoneAnimChannel* FindBoneChannel(const std::string& boneName);

   float GetTicksPerSecond() const { return m_ticksPerSecond; }

   float GetDuration() const { return m_duration; }

private:
   float m_duration;
   int m_ticksPerSecond;
   std::vector<BoneAnimChannel> m_boneChannels;
};

} // namespace BIEngine
