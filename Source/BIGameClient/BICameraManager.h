#pragma once

#include "../BIEngine/Actors/Actor.h"
#include "../BIEngine/Graphics/Camera.h"
#include "../BIEngine/ProcessManager/Process.h"

class BICameraManager {
public:
   BICameraManager(BIEngine::SharedPtr<BIEngine::Camera> pCamera)
      : m_pCamera(pCamera)
   {
   }

   void Terminate();

   void FollowActor(BIEngine::SharedPtr<BIEngine::Actor> pActor);

private:
   void TryCancelCameraFollowProc();

private:
   BIEngine::SharedPtr<BIEngine::Camera> m_pCamera;
   std::shared_ptr<BIEngine::Process> m_pCameraFollowProc;
};
