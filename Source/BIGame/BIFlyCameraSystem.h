#pragma once

#include <memory>

#include "../BIEngine/Graphics/Camera.h"
#include "../BIEngine/Utilities/GameTimer.h"
#include "BIGameController.h"

class BIFlyCameraSystem {
public:
   BIFlyCameraSystem(std::shared_ptr<BIEngine::Camera> pCamera, std::shared_ptr<BIGameController> pInputController);

   float GetMovementSpeed() const { return m_movementSpeed; }

   void SetMovementSpeed(float speed) { m_movementSpeed = speed; }

   void OnUpdate(const BIEngine::GameTimer& gt);

private:
   std::shared_ptr<BIEngine::Camera> m_pCamera;
   std::shared_ptr<BIGameController> m_pInputController;

   float m_movementSpeed;
   float m_rotationSpeed;

   float m_lastPointerX;
   float m_lastPointerY;
};
