#pragma once

#include "BIGCEventListener.h"

namespace BIEngine {
class Camera;
}

class BIInputActionController {
private:
   enum DirectionMask {
      Up = 1,
      Down = 1 << 1,
      Left = 1 << 2,
      Right = 1 << 3
   };

public:
   void Init(int playerId, BIEngine::SharedPtr<BIEngine::Camera> pCamera);
   void Term();

   void OnPointerMoveDelegate(BIEngine::IEventDataPtr pEventData);
   void OnKeyDownDelegate(BIEngine::IEventDataPtr pEventData);
   void OnKeyUpDelegate(BIEngine::IEventDataPtr pEventData);

   void OnUpdate();

private:
   BIEngine::EventManager::DelegateHandler m_onPointerMoveDelegateHandler;
   BIEngine::EventManager::DelegateHandler m_onKeyDownDelegateHandler;
   BIEngine::EventManager::DelegateHandler m_onKeyUpDelegateHandler;

   int m_playerId;

   BIEngine::SharedPtr<BIEngine::Camera> m_pCamera;

   uint8_t m_currentMoveMask = 0u;
   uint8_t m_cachedMoveMask = 0u;

   float m_desiredVerticalAmount = 0.0f;
   float m_desiredHorizontalAmount = 0.0f;
};
