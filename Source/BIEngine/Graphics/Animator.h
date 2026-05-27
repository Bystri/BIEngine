#pragma once

#include "Animation.h"
#include "../Actors/Actor.h"
#include "../Utilities/GameTimer.h"

namespace BIEngine {

class SkeletalModel;
class AnimationPose;

class Animator {
public:
   Animator(Actor* pRoot);

   void Update(float dt);
   void PlayAnimation(SharedPtr<Animation> pAnimation);

private:
   void CalculateAnimationPose(AnimationPose& pose, Actor* pActor);
   void CalculateActorTransform(const AnimationPose& pose, Actor* pActor);

private:
   Actor* m_pRoot;

   SharedPtr<Animation> m_pCurrentAnimation;
   float m_currentTime;
};

} // namespace BIEngine
