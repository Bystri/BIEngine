
#include "ScriptComponent.h"

#include "../Scripting/PythonStateManager.h"
#include "../Utilities/Logger.h"
#include "Actor.h"

namespace BIEngine {

const ComponentId ScriptComponent::g_CompId = "ScriptComponent";

ScriptComponent::ScriptComponent()
{
}

ScriptComponent::~ScriptComponent(void)
{
}

bool ScriptComponent::Init(tinyxml2::XMLElement* pData)
{
   tinyxml2::XMLElement* pScriptObjectElement = pData->FirstChildElement("ScriptObject");
   if (!pScriptObjectElement) {
      Logger::WriteLog(Logger::LogType::ERROR, "No <ScriptObject> tag in XML");
      return false;
   }

   const char* temp = nullptr;

   temp = pScriptObjectElement->Attribute("modulePath");
   if (!temp || strlen(temp) == 0) {
      Logger::WriteLog(Logger::LogType::ERROR, "No module path in <ScriptObject> tag");
      return false;
   }
   m_componentScriptPath = temp;

   temp = pScriptObjectElement->Attribute("className");
   if (!temp || strlen(temp) == 0) {
      Logger::WriteLog(Logger::LogType::ERROR, "No class name in <ScriptObject> tag");
      return false;
   }
   m_className = temp;

   try {
      auto componentModule = pybind11::module_::import(m_componentScriptPath.c_str());
      m_pyObject = componentModule.attr("__dict__").cast<pybind11::dict>()[m_className.c_str()]().cast<pybind11::object>();
      m_pyObject.attr("owner") = m_pOwner;
   } catch (pybind11::error_already_set er) {
      Logger::WriteLog(Logger::LogType::ERROR, std::string("Python error: ") + er.what());
      return false;
   }

   // читаем данные из <StringData>
   tinyxml2::XMLElement* pStringDataElement = pData->FirstChildElement("StringData");
   if (pStringDataElement) {
      for (const tinyxml2::XMLAttribute* pAttribute = pStringDataElement->FirstAttribute(); pAttribute != nullptr; pAttribute = pAttribute->Next()) {
         m_strVars.push_back(std::pair<std::string, std::string>(pAttribute->Name(), pAttribute->Value()));
         m_pyObject.attr(pAttribute->Name()) = pAttribute->Value();
      }
   }

   // читаем данные из <NumberData>
   tinyxml2::XMLElement* pNumberDataElement = pData->FirstChildElement("NumberData");
   if (pNumberDataElement) {
      for (const tinyxml2::XMLAttribute* pAttribute = pNumberDataElement->FirstAttribute(); pAttribute != nullptr; pAttribute = pAttribute->Next()) {
         float val = std::atof(pAttribute->Value());
         m_numVars.push_back(std::pair<std::string, float>(pAttribute->Name(), val));
         m_pyObject.attr(pAttribute->Name()) = val;
      }
   }

   return true;
}

void ScriptComponent::PostInit()
{
   m_pyObject.attr("PostInit")();
}

tinyxml2::XMLElement* ScriptComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   // ScriptObject
   tinyxml2::XMLElement* pScriptObjectElement = pDoc->NewElement("ScriptObject");

   pScriptObjectElement->SetAttribute("modulePath", m_componentScriptPath.c_str());
   pScriptObjectElement->SetAttribute("className", m_className.c_str());
   pBaseElement->LinkEndChild(pScriptObjectElement);

   // StringData
   tinyxml2::XMLElement* pStringData = pDoc->NewElement("StringData");

   for (const auto& data : m_strVars) {
      pStringData->SetAttribute(data.first.c_str(), data.second.c_str());
   }
   pBaseElement->LinkEndChild(pStringData);

   // NumberData
   tinyxml2::XMLElement* pNumberData = pDoc->NewElement("NumberData");

   for (const auto& data : m_numVars) {
      pNumberData->SetAttribute(data.first.c_str(), std::to_string(data.second).c_str());
   }
   pBaseElement->LinkEndChild(pNumberData);

   return pBaseElement;
}

pybind11::object ScriptComponent::GetObject()
{
   return m_pyObject;
}
} // namespace BIEngine
