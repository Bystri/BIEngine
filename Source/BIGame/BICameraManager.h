#pragma once

#include "../BIEngine/Actors/Actor.h"
#include "../BIEngine/Graphics/Camera.h"
#include "../BIEngine/ProcessManager/Process.h"

class BICameraManager {
public:
   BICameraManager(std::shared_ptr<BIEngine::Camera> pCamera)
      : m_pCamera(pCamera)
   {
   }

   void Terminate();

   void FollowActor(std::shared_ptr<BIEngine::Actor> pActor);

private:
   void TryCancelCameraFollowProc();

private:
   std::shared_ptr<BIEngine::Camera> m_pCamera;
   std::shared_ptr<BIEngine::Process> m_pCameraFollowProc;
};
