#include "BIInputActiorController.h"

#include "../BIEngine/Graphics/Camera.h"
#include "../BIEngine/EventManager/EventManager.h"
#include "../BIEngine/EngineCore/GameApp.h"
#include "../BIEngine/Utilities/DebugDraw.h"

void BIInputActionController::Init(int playerId, BIEngine::SharedPtr<BIEngine::Camera> pCamera)
{
   m_playerId = playerId;
   m_pCamera = pCamera;

   m_onPointerMoveDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(BIInputActionController::OnPointerMoveDelegate), EvtData_OnPointerMove::sk_EventType);
   m_onPointerButtonUpDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(BIInputActionController::OnPointerButtonUpDelegate), EvtData_OnPointerButtonUp::sk_EventType);
   m_onKeyDownDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(BIInputActionController::OnKeyDownDelegate), EvtData_OnKeyDown::sk_EventType);
   m_onKeyUpDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(BIInputActionController::OnKeyUpDelegate), EvtData_OnKeyUp::sk_EventType);
}

void BIInputActionController::Term()
{
   BIEngine::EventManager::Get()->RemoveListener(m_onPointerMoveDelegateHandler);
   BIEngine::EventManager::Get()->RemoveListener(m_onPointerButtonUpDelegateHandler);
   BIEngine::EventManager::Get()->RemoveListener(m_onKeyDownDelegateHandler);
   BIEngine::EventManager::Get()->RemoveListener(m_onKeyUpDelegateHandler);
}

void BIInputActionController::OnPointerMoveDelegate(BIEngine::IEventDataPtr pEventData)
{
   BIEngine::SharedPtr<EvtData_OnPointerMove> pCastEventData = BIEngine::StaticPointerCast<EvtData_OnPointerMove>(pEventData);
   glm::vec3 viewportPos = m_pCamera->ScreenToViewportPoint(glm::vec2(pCastEventData->GetPointerPos().x, pCastEventData->GetPointerPos().y));

   viewportPos.z = 0.0f;
   std::swap(viewportPos.x, viewportPos.y);
   viewportPos = glm::normalize(viewportPos);

   BIEngine::SharedPtr<EvtData_Turn> pEvent = BIEngine::MakeShared<EvtData_Turn>(m_playerId, viewportPos);
   BIEngine::EventManager::Get()->QueueEvent(pEvent);
}

void BIInputActionController::OnPointerButtonUpDelegate(BIEngine::IEventDataPtr pEventData)
{
   BIEngine::SharedPtr<EvtData_OnPointerButtonUp> pCastEventData = BIEngine::StaticPointerCast<EvtData_OnPointerButtonUp>(pEventData);

   if (pCastEventData->GetMouseButton() != BIGameController::MouseButton::LEFT) {
      return;
   }

   const glm::vec3 worldPos = m_pCamera->ScreenToWorldPoint(glm::vec2(pCastEventData->GetPointerPos().x, pCastEventData->GetPointerPos().y));

   constexpr float RAYCAST_LENGTH = 100.0f;
   const glm::vec3 raycastDir = glm::normalize(worldPos - m_pCamera->GetPosition());

   const auto raycastInfo = BIEngine::g_pApp->m_pGameLogic->GetGamePhysics3D()->Raycast(m_pCamera->GetPosition(), m_pCamera->GetPosition() + raycastDir * RAYCAST_LENGTH);
   if (raycastInfo.hasHit) {
      BIEngine::DebugDraw::Sphere(raycastInfo.hitPosition, 0.5f, BIEngine::COLOR_GREEN, 5.0f);
   }
}

void BIInputActionController::OnKeyDownDelegate(BIEngine::IEventDataPtr pEventData)
{
   BIEngine::SharedPtr<EvtData_OnKeyDown> pCastEventData = BIEngine::StaticPointerCast<EvtData_OnKeyDown>(pEventData);

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
   BIEngine::SharedPtr<EvtData_OnKeyUp> pCastEventData = BIEngine::StaticPointerCast<EvtData_OnKeyUp>(pEventData);

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
      case EvtData_OnKeyUp::Key::SPACE:
         BIEngine::SharedPtr<EvtData_PrimaryAttack> pEvent = BIEngine::MakeShared<EvtData_PrimaryAttack>(m_playerId);
         BIEngine::EventManager::Get()->QueueEvent(pEvent);
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

      BIEngine::SharedPtr<EvtData_Move> pEvent = BIEngine::MakeShared<EvtData_Move>(m_playerId, m_desiredVerticalAmount, m_desiredHorizontalAmount);
      BIEngine::EventManager::Get()->QueueEvent(pEvent);
   }

   m_cachedMoveMask = m_currentMoveMask;
}
