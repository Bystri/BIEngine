#pragma once

#include <functional>
#include <memory>

#include "../../BIEngine/StdLib/String.h"
#include "../../BIEngine/Renderer/Color.h"

class NumEditWidget {
public:
   NumEditWidget();

   void SetEditCallback(BIEngine::SharedPtr<std::function<void(float)>> onEditCallback)
   {
      m_pOnEditCallback = onEditCallback;
   }

   void SetText(const BIEngine::String& text)
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
   BIEngine::SharedPtr<std::function<void(float)>> m_pOnEditCallback;

   BIEngine::String m_text;
   BIEngine::ColorRgb m_textColor;

   float m_value;
};
