#pragma once

#include "Skeleton.h"
#include "Animation.h"
#include "../Utilities/GameTimer.h"

namespace BIEngine {

class SkeletalModel;

class Animator {
public:
   Animator(std::shared_ptr<SkeletalModel> pModel);

   void Update(float dt);
   void PlayAnimation(std::shared_ptr<Animation> pAnimation);

private:
   void calculateBoneTransform(std::shared_ptr<Skeleton::BoneInfo> node, glm::mat4 parentTransform);

private:
   std::shared_ptr<SkeletalModel> m_pModel;

   std::shared_ptr<Animation> m_pCurrentAnimation;
   float m_currentTime;
};

} // namespace BIEngine
