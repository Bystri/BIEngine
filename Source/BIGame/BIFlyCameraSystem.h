#pragma once

#include "../BIEngine/Graphics/Camera.h"
#include "../BIEngine/Utilities/GameTimer.h"
#include "../BIEngine/EventManager/EventManager.h"

#include "BIGameController.h"

class BIFlyCameraSystem {
public:
   BIFlyCameraSystem(BIEngine::SharedPtr<BIEngine::Camera> pCamera, BIEngine::SharedPtr<BIGameController> pInputController);
   ~BIFlyCameraSystem();

   float GetMovementSpeed() const { return m_movementSpeed; }

   void SetMovementSpeed(float speed) { m_movementSpeed = speed; }

   void OnUpdate(const BIEngine::GameTimer& gt);

private:
   void OnKeyDownCallback(BIEngine::IEventDataPtr pEventData);
   void OnKeyUpCallback(BIEngine::IEventDataPtr pEventData);

private:
   BIEngine::EventManager::DelegateHandler m_onKeyDownDelegateHandler;
   BIEngine::EventManager::DelegateHandler m_onKeyUpDelegateHandler;

   BIEngine::SharedPtr<BIEngine::Camera> m_pCamera;
   BIEngine::SharedPtr<BIGameController> m_pInputController;

   bool m_up;
   bool m_down;
   bool m_left;
   bool m_right;

   float m_movementSpeed;
   float m_rotationSpeed;

   float m_lastPointerX;
   float m_lastPointerY;
};
