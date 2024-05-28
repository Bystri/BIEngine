#pragma once

#include <string>

namespace BIEngine {

class IKeyboardHandler {
public:
   virtual bool OnKeyDown(int key, int scancode) = 0;
   virtual bool OnKeyUp(int key, int scancode) = 0;
   virtual bool IsKeyPressed(int key) const = 0;
};

class IPointerHandler {
public:
   struct Point {
      float x = 0;
      float y = 0;
   };

   virtual bool OnPointerMove(const Point& pos, int radius) = 0;
   virtual bool OnPointerButtonDown(const Point& pos, int radius, int buttonCode) = 0;
   virtual bool OnPointerButtonUp(const Point& pos, int radius, int buttonCode) = 0;
};

class IJoystickHandler {
   virtual bool OnButtonDown(const std::string& buttonName, int pressure) = 0;
   virtual bool OnButtonUp(const std::string& buttonName) = 0;
   virtual bool OnJoystick(float x, float y) = 0;
};

class IGamepadHandler {
   virtual bool OnTrigger(const std::string& triggerName, float pressure) = 0;
   virtual bool OnButtonDown(const std::string& buttonName, int pressure) = 0;
   virtual bool OnButtonUp(const std::string& buttonName) = 0;
   virtual bool OnDirectionalPad(const std::string& direction) = 0;
   virtual bool OnThumbstick(const std::string& stickName, float x, float y) = 0;
};
} // namespace BIEngine
