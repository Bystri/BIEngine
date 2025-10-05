#pragma once

#include <pybind11/pybind11.h>

#include "../StdLib/UniquePtr.h"
#include "ActorComponent.h"
#include "../StdLib/DynamicArray.h"

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

   String m_componentScriptPath;
   String m_className;

   bool m_isExternalScriptUsed;
   String m_externalScriptObjVarName;
   String m_externalScriptObjPath;
   String m_externalScriptObjClass;

   DynamicArray<Pair<String, float>> m_numVars;
   DynamicArray<Pair<String, String>> m_strVars;

   pybind11::object m_pyObject;
};

static UniquePtr<ActorComponent> CreateScriptComponent()
{
   return MakeUnique<ScriptComponent>();
}
} // namespace BIEngine
