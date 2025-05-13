#include "BIInputActiorController.h"

#include "../BIEngine/Graphics/Camera.h"
#include "../BIEngine/EventManager/EventManager.h"

void BIInputActionController::Init(int playerId, std::shared_ptr<BIEngine::Camera> pCamera)
{
   m_playerId = playerId;
   m_pCamera = pCamera;

   BIEngine::EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BIInputActionController::OnPointerMoveDelegate), EvtData_OnPointerMove::sk_EventType);
   BIEngine::EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BIInputActionController::OnKeyDownDelegate), EvtData_OnKeyDown::sk_EventType);
   BIEngine::EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BIInputActionController::OnKeyUpDelegate), EvtData_OnKeyUp::sk_EventType);
}

void BIInputActionController::Term()
{
   BIEngine::EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &BIInputActionController::OnKeyDownDelegate), EvtData_OnPointerMove::sk_EventType);
   BIEngine::EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &BIInputActionController::OnKeyDownDelegate), EvtData_OnKeyDown::sk_EventType);
   BIEngine::EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &BIInputActionController::OnKeyUpDelegate), EvtData_OnKeyUp::sk_EventType);
}

void BIInputActionController::OnPointerMoveDelegate(BIEngine::IEventDataPtr pEventData)
{
   std::shared_ptr<EvtData_OnPointerMove> pCastEventData = std::static_pointer_cast<EvtData_OnPointerMove>(pEventData);
   glm::vec3 worldPos = m_pCamera->ScreenToViewportPoint(glm::vec2(pCastEventData->GetPointerPos().x, pCastEventData->GetPointerPos().y));

   worldPos.z = 0.0f;
   std::swap(worldPos.x, worldPos.y);
   worldPos = glm::normalize(worldPos);

   std::shared_ptr<EvtData_Turn> pEvent = std::make_shared<EvtData_Turn>(m_playerId, worldPos);
   BIEngine::EventManager::Get()->QueueEvent(pEvent);
}

void BIInputActionController::OnKeyDownDelegate(BIEngine::IEventDataPtr pEventData)
{
   std::shared_ptr<EvtData_OnKeyDown> pCastEventData = std::static_pointer_cast<EvtData_OnKeyDown>(pEventData);

   switch (pCastEventData->GetKey()) {
      case EvtData_OnKeyDown::Key::W:
         m_currentMoveMask |= DirectionMask::Up;
         break;
      case EvtData_OnKeyDown::Key::S:
         m_currentMoveMask |= DirectionMask::Down;
         break;
      case EvtData_OnKeyDown::Key::A:
         m_currentMoveMask |= DirectionMask::Left;
         break;
      case EvtData_OnKeyDown::Key::D:
         m_currentMoveMask |= DirectionMask::Right;
         break;
   }
}

void BIInputActionController::OnKeyUpDelegate(BIEngine::IEventDataPtr pEventData)
{
   std::shared_ptr<EvtData_OnKeyUp> pCastEventData = std::static_pointer_cast<EvtData_OnKeyUp>(pEventData);

   switch (pCastEventData->GetKey()) {
      case EvtData_OnKeyUp::Key::W:
         m_currentMoveMask &= 0xff ^ DirectionMask::Up;
         break;
      case EvtData_OnKeyUp::Key::S:
         m_currentMoveMask &= 0xff ^ DirectionMask::Down;
         break;
      case EvtData_OnKeyUp::Key::A:
         m_currentMoveMask &= 0xff ^ DirectionMask::Left;
         break;
      case EvtData_OnKeyUp::Key::D:
         m_currentMoveMask &= 0xff ^ DirectionMask::Right;
         break;
   }
}

void BIInputActionController::OnUpdate()
{
   if (m_cachedMoveMask != m_currentMoveMask) {
      m_desiredVerticalAmount = 0.0f;
      m_desiredHorizontalAmount = 0.0f;

      if (m_currentMoveMask & DirectionMask::Up) {
         m_desiredVerticalAmount += 1.0f;
      }

      if (m_currentMoveMask & DirectionMask::Down) {
         m_desiredVerticalAmount -= 1.0f;
      }

      if (m_currentMoveMask & DirectionMask::Left) {
         m_desiredHorizontalAmount -= 1.0f;
      }

      if (m_currentMoveMask & DirectionMask::Right) {
         m_desiredHorizontalAmount += 1.0f;
      }

      std::shared_ptr<EvtData_Move> pEvent = std::make_shared<EvtData_Move>(m_playerId, m_desiredVerticalAmount, m_desiredHorizontalAmount);
      BIEngine::EventManager::Get()->QueueEvent(pEvent);
   }

   m_cachedMoveMask = m_currentMoveMask;
}
