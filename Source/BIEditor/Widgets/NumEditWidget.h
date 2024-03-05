#pragma once

#include <functional>
#include <string>
#include <memory>

#include "../../BIEngine/Renderer/Color.h"

class NumEditWidget {
public:
   NumEditWidget();

   void SetEditCallback(std::shared_ptr<std::function<void(float)>> onEditCallback)
   {
      m_pOnEditCallback = onEditCallback;
   }

   void SetText(const std::string& text)
   {
      m_text = text;
   }

   void SetTextColor(const BIEngine::ColorRgb& color)
   {
      m_textColor = color;
   }

   void SetValue(float val)
   {
      m_value = val;
   }

   float GetValue() const
   {
      return m_value;
   }

   void Update();

private:
   std::shared_ptr<std::function<void(float)>> m_pOnEditCallback;

   std::string m_text;
   BIEngine::ColorRgb m_textColor;

   float m_value;
};
