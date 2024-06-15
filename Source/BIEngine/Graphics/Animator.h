#pragma once

#include "Animation.h"
#include "../Actors/Actor.h"
#include "../Utilities/GameTimer.h"

namespace BIEngine {

class SkeletalModel;

class Animator {
public:
   Animator(Actor* pRoot);

   void Update(float dt);
   void PlayAnimation(std::shared_ptr<Animation> pAnimation);

private:
   void calculateActorTransform(Actor* pActor);

private:
   Actor* m_pRoot;

   std::shared_ptr<Animation> m_pCurrentAnimation;
   float m_currentTime;
};

} // namespace BIEngine
