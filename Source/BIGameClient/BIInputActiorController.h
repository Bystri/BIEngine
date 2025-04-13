#pragma once

#include "../BIEngine/EventManager/EventManager.h"

#include "BIGCEventListener.h"

class BIInputActionController {
public:
   void Init(int playerId)
   {
      m_playerId = playerId;

      BIEngine::EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BIInputActionController::OnKeyDownDelegate), EvtData_OnKeyDown::sk_EventType);
      BIEngine::EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BIInputActionController::OnKeyUpDelegate), EvtData_OnKeyUp::sk_EventType);
   }

   void Term()
   {
      BIEngine::EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &BIInputActionController::OnKeyDownDelegate), EvtData_OnKeyDown::sk_EventType);
      BIEngine::EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &BIInputActionController::OnKeyUpDelegate), EvtData_OnKeyUp::sk_EventType);
   }

   void OnKeyDownDelegate(BIEngine::IEventDataPtr pEventData)
   {
      BIEngine::Logger::WriteMsgLog("Got key down");
      std::shared_ptr<EvtData_OnKeyDown> pCastEventData = std::static_pointer_cast<EvtData_OnKeyDown>(pEventData);

      switch (pCastEventData->GetKey()) {
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

      m_bIsMoveChanged = m_up || m_down || m_left || m_right;
   }

   void OnKeyUpDelegate(BIEngine::IEventDataPtr pEventData)
   {
      std::shared_ptr<EvtData_OnKeyUp> pCastEventData = std::static_pointer_cast<EvtData_OnKeyUp>(pEventData);

      switch (pCastEventData->GetKey()) {
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

      m_bIsMoveChanged = m_up || m_down || m_left || m_right;
   }

   void OnUpdate()
   {
      if (m_bIsMoveChanged) {
         m_desiredVerticalAmount = 0.0f;
         m_desiredHorizontalAmount = 0.0f;

         if (m_up) {
            m_desiredVerticalAmount += 1.0f;
         }

         if (m_down) {
            m_desiredVerticalAmount -= 1.0f;
         }

         if (m_left) {
            m_desiredHorizontalAmount -= 1.0f;
         }

         if (m_right) {
            m_desiredHorizontalAmount += 1.0f;
         }

         std::shared_ptr<EvtData_Move> pEvent = std::make_shared<EvtData_Move>(m_playerId, m_desiredVerticalAmount, m_desiredHorizontalAmount);
         BIEngine::EventManager::Get()->QueueEvent(pEvent);
      }
   }

private:
   int m_playerId;

   bool m_bIsMoveChanged = false;

   float m_desiredVerticalAmount = 0.0f;
   float m_desiredHorizontalAmount = 0.0f;

   bool m_up;
   bool m_down;
   bool m_left;
   bool m_right;
};
