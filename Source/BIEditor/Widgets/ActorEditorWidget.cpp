#include "ActorEditorWidget.h"

#include <functional>

#include <imgui.h>

#include "../../BIEngine/ResourceCache/XmlLoader.h"
#include "../../BIEngine/EngineCore/GameApp.h"

#include "NumEditWidget.h"

class VecEdit {
public:
   VecEdit(const std::string& componentName, const std::string& settingName, BIEngine::ActorId actorId, tinyxml2::XMLElement* pActorValues)
      : m_settingName(settingName), m_actorId(actorId)
   {
      int i = 0;
      for (auto pAttribute = pActorValues->FirstAttribute(); pAttribute; pAttribute = pAttribute->Next()) {
         m_editWidgets[i].SetValue(pAttribute->FloatValue());

         std::string parameterName = pAttribute->Name();
         m_editWidgets[i].SetText(parameterName);
         m_editWidgets[i].SetTextColor(BIEngine::ColorRgb(0.1f + i * 0.45f, 0.1f + i * 0.35f, 0.8f));

         m_editWidgets[i].SetEditCallback(
            std::make_shared<std::function<void(float)>>([actorId = m_actorId, componentName = componentName, settingName = m_settingName, parameterName](float value) { numEdited(actorId, componentName, settingName, parameterName, value); }));

         ++i;
      }
   }

   void Show()
   {
      if (ImGui::TreeNodeEx(m_settingName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
         for (int i = 0; i < 3; ++i) {
            m_editWidgets[i].Update();
         }

         ImGui::TreePop();
      }
   }

private:
   static void numEdited(BIEngine::ActorId actorId, const std::string& componentName, const std::string& settingName, const std::string& parameterName, float value)
   {
      tinyxml2::XMLDocument changedActorParametrs;
      tinyxml2::XMLElement* const pRoot = changedActorParametrs.NewElement("Actor");
      tinyxml2::XMLElement* const pRootComponents = changedActorParametrs.NewElement("Components");
      pRoot->LinkEndChild(pRootComponents);


      const std::shared_ptr<BIEngine::Actor> pActor = BIEngine::g_pApp->m_pGameLogic->GetActor(actorId);

      tinyxml2::XMLDocument actorXmlDoc;
      tinyxml2::XMLElement* const pActorElement = pActor->ToXML(&actorXmlDoc);
      actorXmlDoc.LinkEndChild(pActorElement);

      tinyxml2::XMLElement* const pComponentsElement = actorXmlDoc.RootElement()->FirstChildElement("Components");
      for (tinyxml2::XMLElement* pComponentNode = pComponentsElement->FirstChildElement(); pComponentNode; pComponentNode = pComponentNode->NextSiblingElement()) {
         if (pComponentNode->Name() != componentName) {
            continue;
         }

         tinyxml2::XMLElement* const pNewComponentNode = pComponentNode->DeepClone(&changedActorParametrs)->ToElement();
         pNewComponentNode->FirstChildElement(settingName.c_str())->SetAttribute(parameterName.c_str(), value);

         pRootComponents->LinkEndChild(pNewComponentNode);
         BIEngine::g_pApp->m_pGameLogic->ModifyActor(actorId, pRoot);
         return;
      }
   }

private:
   std::string m_settingName;

   BIEngine::ActorId m_actorId;

   NumEditWidget m_editWidgets[3];
};

class ActorComponentEdit {
public:
   ActorComponentEdit(BIEngine::ActorId actorId, tinyxml2::XMLElement* actorComponentValues, tinyxml2::XMLElement* editorComponentValues)
      : m_actorId(actorId)
   {
      const char* componentName;
      editorComponentValues->QueryStringAttribute("name", &componentName);
      m_componentName = componentName;

      for (tinyxml2::XMLElement* pNode = editorComponentValues->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
         const char *namecstr, *typecstr;
         pNode->QueryStringAttribute("name", &namecstr);
         pNode->QueryStringAttribute("type", &typecstr);
         std::string elementName = namecstr;
         std::string elementType = typecstr;

         tinyxml2::XMLElement* pActorValues = actorComponentValues->FirstChildElement(namecstr);

         if (elementType == "Vec3") {
            m_vectors.push_back(VecEdit(componentName, elementName, actorId, pActorValues));
         }
      }
   }

   void Show()
   {
      if (!ImGui::CollapsingHeader(m_componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
         return;
      }

      for (int i = 0; i < m_vectors.size(); ++i) {
         m_vectors[i].Show();
      }
   }

private:
   BIEngine::ActorId m_actorId;

   std::string m_componentName;

   std::vector<VecEdit> m_vectors;
};

ActorEditorWidget::ActorEditorWidget()
   : m_currentActorId(-1)
{
   auto pXmlExtraData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle("editor/components.xml")->GetExtra());

   tinyxml2::XMLElement* pComponentsNode = pXmlExtraData->GetRootElement();
   if (pComponentsNode) {
      for (tinyxml2::XMLElement* pNode = pComponentsNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
         const char* namecstr;
         pNode->QueryStringAttribute("name", &namecstr);
         std::string elementName = namecstr;
         m_componentsSettings[elementName] = pNode;
      }
   }
}

void ActorEditorWidget::SetCurrentEditableActorId(BIEngine::ActorId actorId)
{
   if (m_currentActorId == actorId) {
      return;
   }

   m_currentActorId = actorId;

   for (int i = 0; i < m_actorComponentEdits.size(); ++i) {
      delete m_actorComponentEdits[i];
   }

   m_actorComponentEdits.clear();

   std::shared_ptr<BIEngine::Actor> pActor = BIEngine::g_pApp->m_pGameLogic->GetActor(m_currentActorId);

   tinyxml2::XMLDocument actorXmlDoc;
   tinyxml2::XMLElement* const pActorElement = pActor->ToXML(&actorXmlDoc);
   actorXmlDoc.LinkEndChild(pActorElement);

   tinyxml2::XMLElement* const pComponentsElement = pActorElement->FirstChildElement("Components");
   for (tinyxml2::XMLElement* pNode = pComponentsElement->LastChildElement(); pNode; pNode = pNode->PreviousSiblingElement()) {
      auto itr = m_componentsSettings.find(pNode->Name());

      if (itr != m_componentsSettings.end()) {
         m_actorComponentEdits.push_back(new ActorComponentEdit(m_currentActorId, pNode, m_componentsSettings[pNode->Name()]));
      }
   }
}

void ActorEditorWidget::Show()
{
   std::shared_ptr<BIEngine::Actor> pActor = BIEngine::g_pApp->m_pGameLogic->GetActor(m_currentActorId);

   tinyxml2::XMLDocument actorXmlDoc;
   tinyxml2::XMLElement* const pActorElement = pActor->ToXML(&actorXmlDoc);
   actorXmlDoc.LinkEndChild(pActorElement);

   if (ImGui::Begin("Actor Editor")) {
      ImGui::PushID(m_currentActorId);

      for (int i = 0; i < m_actorComponentEdits.size(); ++i) {
         m_actorComponentEdits[i]->Show();
      }

      ImGui::PopID();
      ImGui::End();
   }
}
