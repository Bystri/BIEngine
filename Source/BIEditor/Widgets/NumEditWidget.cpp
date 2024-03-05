#include "NumEditWidget.h"

#include <imgui.h>

NumEditWidget::NumEditWidget()
   : m_pOnEditCallback(nullptr), m_text(), m_textColor(0.2f, 0.2f, 1.0f), m_value(0.0f)
{
}

void NumEditWidget::Update()
{
   ImGui::PushID(m_text.c_str());

   ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(m_textColor.r, m_textColor.g, m_textColor.b));
   ImGui::BeginDisabled();
   ImGui::Button(m_text.c_str());
   ImGui::EndDisabled();
   ImGui::PopStyleColor(1);

   ImGui::SameLine();

   if (ImGui::InputFloat("", &m_value)) {
      (*m_pOnEditCallback)(m_value);
   }
   ImGui::PopID();
}
