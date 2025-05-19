#pragma once

#include <imgui.h>

class BIDebugMenuController {
public:
   bool IsShowNavMeshWindow() const { return m_showNavMeshWindow; }

   bool IsShowPhysics3dWindow() const { return m_showPhysics3dWindow; }

   void OnUpdate()
   {
      if (ImGui::BeginMainMenuBar()) {
         if (ImGui::BeginMenu("Engine")) {
            ImGui::MenuItem("NavMesh", nullptr, &m_showNavMeshWindow);
            ImGui::MenuItem("Physics3d", nullptr, &m_showPhysics3dWindow);
            ImGui::EndMenu();
         }
         ImGui::EndMainMenuBar();
      }
   }

private:
   bool m_showNavMeshWindow = false;
   bool m_showPhysics3dWindow = false;
};
