#pragma once

#include <string>
#include <unordered_map>

#include <tinyxml2.h>
#include <imgui.h>

#include "../../BIEngine/Actors/Actor.h"
#include "../../BIEngine/EngineCore/GameApp.h"

class ActorComponentEdit;

class ActorEditorWidget {
public:
   ActorEditorWidget();

   void SetCurrentEditableActorId(BIEngine::ActorId actorId);
   void Show();

private:
   BIEngine::ActorId m_currentActorId;
   std::vector<ActorComponentEdit*> m_actorComponentEdits;

   std::unordered_map<std::string, tinyxml2::XMLElement*> m_componentsSettings;
};
