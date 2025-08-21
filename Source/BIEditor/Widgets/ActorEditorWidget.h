#pragma once

#include <tinyxml2.h>
#include <imgui.h>

#include "../../BIEngine/StdLib/String.h"
#include "../../BIEngine/StdLib/HashMap.h"
#include "../../BIEngine/Actors/Actor.h"
#include "../../BIEngine/EngineCore/GameApp.h"

class ActorComponentEdit;

class ActorEditorWidget {
public:
   ActorEditorWidget();

   BIEngine::ActorId GetCurrentSelectedActorId() const { return m_currentActorId; }

   void SetCurrentEditableActorId(BIEngine::ActorId actorId);
   void Show();

private:
   BIEngine::ActorId m_currentActorId;
   BIEngine::DynamicArray<ActorComponentEdit*> m_actorComponentEdits;

   BIEngine::HashMap<BIEngine::String, tinyxml2::XMLElement*> m_componentsSettings;
};
