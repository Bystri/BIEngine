#pragma once

#include "../BIEngine/UserInterface/InputDevices.h"

const int MAX_NUMBER_OF_KEYS = 1024;

class BIGameController : public BIEngine::IPointerHandler, public BIEngine::IKeyboardHandler {
public:
   enum class MouseButton {
      LEFT,
      RIGHT,
      MIDDLE
   };

   BIGameController()
      : m_currentPointerPos{0.0, 0.0}, m_isKeyPressed()
   {
   }

public:
   IPointerHandler::Point GetCurrentPointerPos() const { return m_currentPointerPos; }

   virtual bool OnPointerMove(const Point& mousePos, const int radius) override;

   virtual bool OnPointerButtonDown(const Point& mousePos, const int radius, int buttonCode) override;

   virtual bool OnPointerButtonUp(const Point& mousePos, const int radius, int buttonCode) override;

   bool IsKeyPressed(int key) const;

   bool IsMouseButtonPressed(int buttonCode);

   virtual bool OnKeyDown(int key, int scancode) override;
   virtual bool OnKeyUp(int key, int scancode) override;

protected:
   IPointerHandler::Point m_currentPointerPos;

   // Состояние клавиш клавиатуры
   bool m_isKeyPressed[MAX_NUMBER_OF_KEYS];

   unsigned char m_mouseButtonsStatus;
};
