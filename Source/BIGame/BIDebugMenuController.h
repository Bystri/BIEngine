#pragma once

#include <imgui.h>

class BIDebugMenuController {
public:
   bool IsShowNavMeshWindow() const { return m_showNavMeshWindow; }

   bool IsShowPhysics3dWindow() const { return m_showPhysics3dWindow; }

   bool IsNetworkWindow() const { return m_showNetworkWindow; }

   void OnUpdate()
   {
      if (ImGui::BeginMainMenuBar()) {
         if (ImGui::BeginMenu("Engine")) {
            ImGui::MenuItem("NavMesh", nullptr, &m_showNavMeshWindow);
            ImGui::MenuItem("Physics3d", nullptr, &m_showPhysics3dWindow);
            ImGui::MenuItem("Network", nullptr, &m_showNetworkWindow);
            ImGui::EndMenu();
         }
         ImGui::EndMainMenuBar();
      }
   }

private:
   bool m_showNavMeshWindow = false;
   bool m_showPhysics3dWindow = false;
   bool m_showNetworkWindow = false;
};
