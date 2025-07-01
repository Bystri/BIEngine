#pragma once

#include <string>

#include <glm/glm.hpp>

#include "../StdLib/DynamicArray.h"
#include "BoneAnimChannel.h"

namespace BIEngine {

class Animation {
public:
   Animation(float duration, int ticksPerSecond, DynamicArray<BoneAnimChannel> boneChannels, bool isLooped = false)
      : m_duration(duration), m_ticksPerSecond(ticksPerSecond), m_boneChannels(std::move(boneChannels)), m_isLooped(isLooped)
   {
   }

   BoneAnimChannel* FindBoneChannel(const std::string& boneName);

   float GetTicksPerSecond() const { return m_ticksPerSecond; }

   float GetDuration() const { return m_duration; }

   bool IsLooped() const { return m_isLooped; }

private:
   float m_duration;
   int m_ticksPerSecond;
   DynamicArray<BoneAnimChannel> m_boneChannels;
   bool m_isLooped;
};

} // namespace BIEngine
