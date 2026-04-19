#pragma once

#include <imgui.h>

class BIDebugMenuController {
public:
   bool IsShowNavMeshWindow() const { return m_showNavMeshWindow; }

   bool IsNavCrowdWindow() const { return m_showNavCrowdWindow; }

   bool IsShowPhysics3dWindow() const { return m_showPhysics3dWindow; }

   bool IsNetworkWindow() const { return m_showNetworkWindow; }

   bool IsReplicationWindow() const { return m_showReplicationWindow; }

   void OnUpdate()
   {
      if (ImGui::BeginMainMenuBar()) {
         if (ImGui::BeginMenu("Engine")) {
            ImGui::MenuItem("NavMesh", nullptr, &m_showNavMeshWindow);
            ImGui::MenuItem("NavCrowd", nullptr, &m_showNavCrowdWindow);
            ImGui::MenuItem("Physics3d", nullptr, &m_showPhysics3dWindow);
            ImGui::MenuItem("Network", nullptr, &m_showNetworkWindow);
            ImGui::MenuItem("Replication", nullptr, &m_showReplicationWindow);
            ImGui::EndMenu();
         }
         ImGui::EndMainMenuBar();
      }
   }

private:
   bool m_showNavMeshWindow = false;
   bool m_showNavCrowdWindow = false;
   bool m_showPhysics3dWindow = false;
   bool m_showNetworkWindow = false;
   bool m_showReplicationWindow = false;
};
