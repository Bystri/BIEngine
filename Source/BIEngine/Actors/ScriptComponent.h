#pragma once

#include <pybind11/pybind11.h>

#include "ActorComponent.h"

namespace BIEngine {
class ScriptComponent : public ActorComponent {
public:
   static const ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return m_className; };

public:
   ScriptComponent();
   virtual ~ScriptComponent();

   virtual bool Init(tinyxml2::XMLElement* pData) override;
   virtual void Activate() override;
   virtual void Deactivate() override;
   virtual void Terminate() override;

   virtual void OnUpdate(const GameTimer& gt) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   pybind11::object GetObject();

private:
   bool m_isScriptInited;

   std::string m_componentScriptPath;
   std::string m_className;

   bool m_isExternalScriptUsed;
   std::string m_externalScriptObjVarName;
   std::string m_externalScriptObjPath;
   std::string m_externalScriptObjClass;

   std::vector<std::pair<std::string, float>> m_numVars;
   std::vector<std::pair<std::string, std::string>> m_strVars;

   pybind11::object m_pyObject;
};

static std::unique_ptr<ActorComponent> CreateScriptComponent()
{
   return std::make_unique<ScriptComponent>();
}
} // namespace BIEngine
