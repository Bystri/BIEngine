#include "BIFlyCameraSystem.h"

#include "../BIEngine/EngineCore/GameApp.h"

#include "BIEventListener.h"

BIFlyCameraSystem::BIFlyCameraSystem(std::shared_ptr<BIEngine::Camera> pCamera, std::shared_ptr<BIGameController> pInputController)
   : m_pCamera(pCamera), m_pInputController(pInputController),
     m_movementSpeed(10.0f), m_rotationSpeed(0.5f),
     m_up(false), m_down(false), m_left(false), m_right(false),
     m_lastPointerX(BIEngine::g_pApp->m_options.screenWidth / 2.0f), m_lastPointerY(BIEngine::g_pApp->m_options.screenHeight / 2.0f)
{
   BIEngine::EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BIFlyCameraSystem::OnKeyDownCallback), EvtData_OnKeyDown::sk_EventType);
   BIEngine::EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BIFlyCameraSystem::OnKeyUpCallback), EvtData_OnKeyUp::sk_EventType);
}

BIFlyCameraSystem::~BIFlyCameraSystem()
{
   BIEngine::EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &BIFlyCameraSystem::OnKeyDownCallback), EvtData_OnKeyDown::sk_EventType);
   BIEngine::EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &BIFlyCameraSystem::OnKeyUpCallback), EvtData_OnKeyUp::sk_EventType);
}

void BIFlyCameraSystem::OnKeyDownCallback(BIEngine::IEventDataPtr pEventData)
{
   std::shared_ptr<EvtData_OnKeyDown> pCastEventData = std::static_pointer_cast<EvtData_OnKeyDown>(pEventData);
   const EvtData_OnKeyDown::Key key = pCastEventData->GetKey();

   switch (key) {
      case EvtData_OnKeyDown::Key::W:
         m_up = true;
         break;
      case EvtData_OnKeyDown::Key::S:
         m_down = true;
         break;
      case EvtData_OnKeyDown::Key::A:
         m_left = true;
         break;
      case EvtData_OnKeyDown::Key::D:
         m_right = true;
         break;
   }
}

void BIFlyCameraSystem::OnKeyUpCallback(BIEngine::IEventDataPtr pEventData)
{
   std::shared_ptr<EvtData_OnKeyUp> pCastEventData = std::static_pointer_cast<EvtData_OnKeyUp>(pEventData);
   const EvtData_OnKeyUp::Key key = pCastEventData->GetKey();

   switch (key) {
      case EvtData_OnKeyUp::Key::W:
         m_up = false;
         break;
      case EvtData_OnKeyUp::Key::S:
         m_down = false;
         break;
      case EvtData_OnKeyUp::Key::A:
         m_left = false;
         break;
      case EvtData_OnKeyUp::Key::D:
         m_right = false;
         break;
   }
}

void BIFlyCameraSystem::OnUpdate(const BIEngine::GameTimer& gt)
{
   const float speed = m_movementSpeed * gt.DeltaTime();

   if (m_up) {
      m_pCamera->MoveRelForward(speed);
   }

   if (m_down) {
      m_pCamera->MoveRelForward(-speed);
   }

   if (m_left) {
      m_pCamera->MoveRelSide(-speed);
   }

   if (m_right) {
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
