#pragma once

#include "AnimationPose.h"
#include "../Actors/Actor.h"

namespace BIEngine {

class AnimationBlender {
public:
   void BlendPoses(Actor* pActor, const AnimationPose& posA, const AnimationPose& posB, float weightA, AnimationPose& result);
};

} // namespace BIEngine