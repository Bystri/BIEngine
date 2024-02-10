#include "BIFlyCameraSystem.h"

#include "../BIEngine/EngineCore/GameApp.h"

BIFlyCameraSystem::BIFlyCameraSystem(std::shared_ptr<BIEngine::Camera> pCamera, std::shared_ptr<BIGameController> pInputController)
   : m_pCamera(pCamera), m_pInputController(pInputController)

     ,
     m_movementSpeed(10.0f), m_rotationSpeed(0.5f)

     ,
     m_lastPointerX(BIEngine::g_pApp->m_options.screenWidth / 2.0f), m_lastPointerY(BIEngine::g_pApp->m_options.screenHeight / 2.0f)
{
}

void BIFlyCameraSystem::OnUpdate(const BIEngine::GameTimer& gt)
{
   const float speed = m_movementSpeed * gt.DeltaTime();
   if (m_pInputController->IsKeyPressed(static_cast<int>(BIGameController::KEY::W))) {
      m_pCamera->MoveRelForward(speed);
   } else if (m_pInputController->IsKeyPressed(static_cast<int>(BIGameController::KEY::S))) {
      m_pCamera->MoveRelForward(-speed);
   } else if (m_pInputController->IsKeyPressed(static_cast<int>(BIGameController::KEY::A))) {
      m_pCamera->MoveRelSide(-speed);
   } else if (m_pInputController->IsKeyPressed(static_cast<int>(BIGameController::KEY::D))) {
      m_pCamera->MoveRelSide(speed);
   }

   const float xoffset = m_pInputController->GetCurrentPointerPos().x - m_lastPointerX;
   const float yoffset = m_lastPointerY - m_pInputController->GetCurrentPointerPos().y;

   m_lastPointerX = m_pInputController->GetCurrentPointerPos().x;
   m_lastPointerY = m_pInputController->GetCurrentPointerPos().y;

   if (!m_pInputController->IsMouseButtonPressed(static_cast<int>(BIGameController::MouseButton::RIGHT))) {
      return;
   }

   m_pCamera->TurnRelAroundY(yoffset * m_rotationSpeed);
   m_pCamera->TurnRelAroundZ(xoffset * m_rotationSpeed);

   if (m_pCamera->GetRotationY() > 89.0f) {
      m_pCamera->SetRotationY(89.0f);
   } else if (m_pCamera->GetRotationY() < -89.0f) {
      m_pCamera->SetRotationY(-89.0f);
   }
}
