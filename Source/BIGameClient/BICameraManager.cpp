#include "BICameraManager.h"

#include "../BIEngine/EngineCore/Assert.h"
#include "../BIEngine/Actors/TransformComponent.h"
#include "../BIEngine/Graphics/Camera.h"
#include "../BIEngine/Math/Vector.h"
#include "../BIEngine/ProcessManager/ProcessManager.h"

class CameraFollowProcess : public BIEngine::Process {
public:
   CameraFollowProcess(std::shared_ptr<BIEngine::Camera> pCamera, std::weak_ptr<BIEngine::TransformComponent> pTransform, float dist, float height)
      : BIEngine::Process(), m_pTransform(pTransform), m_pCamera(pCamera), m_dist(dist), m_height(height)
   {
   }

   virtual void OnUpdate(float dt) override
   {
      std::shared_ptr<BIEngine::TransformComponent> pTransform = m_pTransform.lock();
      BIEngine::Assert(pTransform != nullptr, "Transform of actor is null");

      constexpr glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

      const glm::vec3 pos = pTransform->GetPosition();
      const glm::vec3 dir = pTransform->GetDir();

      const glm::vec3 eyePos = pos - dir * m_dist + up * m_height;
      const glm::vec3 camForward = glm::normalize(pos - eyePos);

      const glm::vec3 camLeft = glm::normalize(glm::cross(up, camForward));
      const glm::vec3 camUp = glm::normalize(glm::cross(camForward, camLeft));

      m_pCamera->LookAt(eyePos, camForward, camUp);
   }

private:
   std::shared_ptr<BIEngine::Camera> m_pCamera;
   std::weak_ptr<BIEngine::TransformComponent> m_pTransform;

   float m_dist;
   float m_height;
};

class StaticCameraFollowProcess : public BIEngine::Process {
public:
   StaticCameraFollowProcess(std::shared_ptr<BIEngine::Camera> pCamera, std::weak_ptr<BIEngine::TransformComponent> pTransform, float dist, float height)
      : BIEngine::Process(), m_pTransform(pTransform), m_pCamera(pCamera), m_dist(dist), m_height(height)
   {
   }

   virtual void OnUpdate(float dt) override
   {
      std::shared_ptr<BIEngine::TransformComponent> pTransform = m_pTransform.lock();
      BIEngine::Assert(pTransform != nullptr, "Transform of actor is null");

      constexpr glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
      constexpr glm::vec3 dir = glm::vec3(1.0f, 0.0f, 0.0f);

      const glm::vec3 pos = pTransform->GetPosition();

      const glm::vec3 eyePos = pos - dir * m_dist + up * m_height;
      const glm::vec3 camForward = glm::normalize(pos - eyePos);

      const glm::vec3 camLeft = glm::normalize(glm::cross(up, camForward));
      const glm::vec3 camUp = glm::normalize(glm::cross(camForward, camLeft));

      m_pCamera->LookAt(eyePos, camForward, camUp);
   }

private:
   std::shared_ptr<BIEngine::Camera> m_pCamera;
   std::weak_ptr<BIEngine::TransformComponent> m_pTransform;

   float m_dist;
   float m_height;
};

void BICameraManager::Terminate()
{
   TryCancelCameraFollowProc();
}

void BICameraManager::FollowActor(std::shared_ptr<BIEngine::Actor> pActor)
{
   TryCancelCameraFollowProc();

   std::weak_ptr<BIEngine::TransformComponent> pTransform = pActor->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId);
   m_pCameraFollowProc = std::make_shared<StaticCameraFollowProcess>(m_pCamera, pTransform, 15.0f, 15.0f);

   BIEngine::ProcessManager::Get()->AttachProcess(m_pCameraFollowProc);
}

void BICameraManager::TryCancelCameraFollowProc()
{
   if (m_pCameraFollowProc == nullptr) {
      return;
   }

   m_pCameraFollowProc->Fail();
}
