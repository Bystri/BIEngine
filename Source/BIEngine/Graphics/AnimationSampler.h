#pragma once

#include "Animation.h"
#include "AnimationPose.h"
#include "../Actors/Actor.h"
#include "../StdLib/SharedPtr.h"

namespace BIEngine {

class AnimationSampler {
public:
   AnimationSampler(SharedPtr<Animation> pCurrentAnimation)
      : m_pCurrentAnimation(pCurrentAnimation)
   {
   }

   void CalculatePoseForActor(Actor* pRoot, AnimationPose& pose, float dt);

private:
   void CalculateAnimationPose(AnimationPose& pose, Actor* pActor);

private:
   SharedPtr<Animation> m_pCurrentAnimation;
   float m_currentTime = 0.0f;
};

} // namespace BIEngine