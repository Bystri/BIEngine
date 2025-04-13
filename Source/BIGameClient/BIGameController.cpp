#include "BIGameController.h"

#include "../BIEngine/EngineCore/Assert.h"

#include "BIGCEventListener.h"

bool BIGameController::OnPointerMove(const Point& mousePos, const int radius)
{
   m_currentPointerPos = mousePos;

   return true;
}

static void gameControllerSetMouseButtonStatus(unsigned char& statuses, BIGameController::MouseButton mouseButton)
{
   const int statusIdx = static_cast<int>(mouseButton);
   statuses |= (1u << statusIdx);
}

static void gameControllerClearMouseButtonStatus(unsigned char& statuses, BIGameController::MouseButton mouseButton)
{
   const int statusIdx = static_cast<int>(mouseButton);
   statuses &= ~(1u << statusIdx);
}

bool BIGameController::OnPointerButtonDown(const Point& mousePos, const int radius, int buttonCode)
{
   gameControllerSetMouseButtonStatus(m_mouseButtonsStatus, static_cast<BIGameController::MouseButton>(buttonCode));

   return true;
}

bool BIGameController::OnPointerButtonUp(const Point& mousePos, const int radius, int buttonCode)
{
   gameControllerClearMouseButtonStatus(m_mouseButtonsStatus, static_cast<BIGameController::MouseButton>(buttonCode));

   return true;
}

bool BIGameController::IsMouseButtonPressed(int buttonCode)
{
   return (m_mouseButtonsStatus & (1u << buttonCode));
}

bool BIGameController::IsKeyPressed(int key) const
{
   BIEngine::Assert(key >= 0 && key < MAX_NUMBER_OF_KEYS, "Get incorrect key code! Got %d but max code is %d", key, MAX_NUMBER_OF_KEYS);
   if (key < 0 || key >= MAX_NUMBER_OF_KEYS) {
      return false;
   }

   return m_isKeyPressed[key];
}

bool BIGameController::OnKeyDown(int key, int scancode)
{
   BIEngine::Assert(key >= 0 && key < MAX_NUMBER_OF_KEYS, "Get incorrect key code! Got %d but max code is %d", key, MAX_NUMBER_OF_KEYS);
   if (key < 0 || key >= MAX_NUMBER_OF_KEYS) {
      return false;
   }

   // Обновляем таблицу клавиш
   m_isKeyPressed[key] = true;

   std::shared_ptr<EvtData_OnKeyDown> pEvent = std::make_shared<EvtData_OnKeyDown>(0, static_cast<EvtData_OnKeyEvent::Key>(key));
   BIEngine::EventManager::Get()->QueueEvent(pEvent);

   return true;
}

bool BIGameController::OnKeyUp(int key, int scancode)
{
   BIEngine::Assert(key >= 0 && key < MAX_NUMBER_OF_KEYS, "Get incorrect key code! Got %d but max code is %d", key, MAX_NUMBER_OF_KEYS);
   if (key < 0 || key >= MAX_NUMBER_OF_KEYS) {
      return false;
   }

   // Обновляем таблицу клавиш
   m_isKeyPressed[key] = false;

   std::shared_ptr<EvtData_OnKeyUp> pEvent = std::make_shared<EvtData_OnKeyUp>(0, static_cast<EvtData_OnKeyEvent::Key>(key));
   BIEngine::EventManager::Get()->QueueEvent(pEvent);

   return true;
}
