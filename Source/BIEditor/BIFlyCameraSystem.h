#pragma once

#include <memory>

#include "BIEditorController.h"
#include "../BIEngine/Graphics/Camera.h"
#include "../BIEngine/Utilities/GameTimer.h"
#include "../BIEngine/EventManager/EventManager.h"

class BIFlyCameraSystem {
public:
   BIFlyCameraSystem(std::shared_ptr<BIEngine::Camera> pCamera, std::shared_ptr<BIEditorController> pInputController);
   ~BIFlyCameraSystem();

   float GetMovementSpeed() const { return m_movementSpeed; }

   void SetMovementSpeed(float speed) { m_movementSpeed = speed; }

   void OnUpdate(const BIEngine::GameTimer& gt);

private:
   void OnKeyDownCallback(BIEngine::IEventDataPtr pEventData);
   void OnKeyUpCallback(BIEngine::IEventDataPtr pEventData);

private:
   std::shared_ptr<BIEngine::Camera> m_pCamera;
   std::shared_ptr<BIEditorController> m_pInputController;

   bool m_up;
   bool m_down;
   bool m_left;
   bool m_right;

   float m_movementSpeed;
   float m_rotationSpeed;

   float m_lastPointerX;
   float m_lastPointerY;
};
