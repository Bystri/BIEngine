#pragma once

#include "Animation.h"
#include "../Actors/Actor.h"
#include "../Utilities/GameTimer.h"

namespace BIEngine {

class SkeletalModel;
class AnimationPose;
class AnimationSampler;

class Animator {
public:
   Animator(Actor* pRoot);

   void Update(float dt);
   void PlayAnimation(SharedPtr<Animation> pAnimation);

private:
   void CalculateActorTransform(const AnimationPose& pose, Actor* pActor);

private:
   Actor* m_pRoot;

   SharedPtr<AnimationSampler> m_pAnimationSampler;
   float m_currentTime;
};

} // namespace BIEngine
