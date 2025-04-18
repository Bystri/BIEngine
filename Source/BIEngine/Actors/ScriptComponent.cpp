﻿
#include "ScriptComponent.h"

#include "../EngineCore/GameApp.h"
#include "../Scripting/PythonStateManager.h"
#include "../Utilities/Logger.h"
#include "Actor.h"

namespace BIEngine {

const ComponentId ScriptComponent::g_CompId = "ScriptComponent";

ScriptComponent::ScriptComponent()
   : m_isScriptInited(false),

     m_isExternalScriptUsed(false),
     m_externalScriptObjVarName(),
     m_externalScriptObjPath(),
     m_externalScriptObjClass()
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
      m_pyObject.attr("owner") = PythonStateManager::RawPtrWrapper<Actor>(m_pOwner);
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

   // читаем данные из <ExternalScriptObj>
   tinyxml2::XMLElement* pExternalScriptObject = pData->FirstChildElement("ExternalScriptObj");
   if (pExternalScriptObject) {
      temp = pExternalScriptObject->Attribute("varName");
      if (!temp || strlen(temp) == 0) {
         Logger::WriteLog(Logger::LogType::ERROR, "No variable name in <ExternalScriptObj> tag");
         return false;
      }
      m_externalScriptObjVarName = temp;


      temp = pExternalScriptObject->Attribute("modulePath");
      if (!temp || strlen(temp) == 0) {
         Logger::WriteLog(Logger::LogType::ERROR, "No module path in <ExternalScriptObj> tag");
         return false;
      }
      m_externalScriptObjPath = temp;

      temp = pExternalScriptObject->Attribute("className");
      if (!temp || strlen(temp) == 0) {
         Logger::WriteLog(Logger::LogType::ERROR, "No class name in <ExternalScriptObj> tag");
         return false;
      }
      m_externalScriptObjClass = temp;

      try {
         py::object classInstance = py::module::import(m_externalScriptObjPath.c_str()).attr(m_externalScriptObjClass.c_str())();
         m_pyObject.attr(m_externalScriptObjVarName.c_str()) = classInstance;
      } catch (pybind11::error_already_set er) {
         Logger::WriteLog(Logger::LogType::ERROR, std::string("Python error: ") + er.what());
         return false;
      }

      m_isExternalScriptUsed = true;
   }

   return true;
}

void ScriptComponent::Activate()
{
   if (g_pApp->m_options.isEditorMode) {
      return;
   }

   if (!m_isScriptInited) {
      try {
         if (py::hasattr(m_pyObject, "OnInit")) {
            m_pyObject.attr("OnInit")();
         }
      } catch (pybind11::error_already_set er) {
         Logger::WriteLog(Logger::LogType::ERROR, std::string("Python error while OnInit: ") + er.what());
      }

      m_isScriptInited = true;
   }
   try {
      if (py::hasattr(m_pyObject, "OnActivate")) {
         m_pyObject.attr("OnActivate")();
      }
   } catch (pybind11::error_already_set er) {
      Logger::WriteLog(Logger::LogType::ERROR, std::string("Python error while OnActivate: ") + er.what());
   }
}

void ScriptComponent::Deactivate()
{
   if (g_pApp->m_options.isEditorMode) {
      return;
   }

   try {
      if (py::hasattr(m_pyObject, "OnDeactivate")) {
         m_pyObject.attr("OnDeactivate")();
      }
   } catch (pybind11::error_already_set er) {
      Logger::WriteLog(Logger::LogType::ERROR, std::string("Python error while OnDeactivate: ") + er.what());
   }
}

void ScriptComponent::Terminate()
{
   if (g_pApp->m_options.isEditorMode) {
      return;
   }

   try {
      if (py::hasattr(m_pyObject, "OnTerminate")) {
         m_pyObject.attr("OnTerminate")();
      }
   } catch (pybind11::error_already_set er) {
      Logger::WriteLog(Logger::LogType::ERROR, std::string("Python error while OnTerminate: ") + er.what());
   }
}

void ScriptComponent::OnUpdate(const GameTimer& gt)
{
   if (g_pApp->m_options.isEditorMode) {
      return;
   }

   try {
      if (py::hasattr(m_pyObject, "OnUpdate")) {
         m_pyObject.attr("OnUpdate")(gt.DeltaTime());
      }
   } catch (pybind11::error_already_set er) {
      Logger::WriteLog(Logger::LogType::ERROR, std::string("Python error while OnUpdate: ") + er.what());
   }
}

tinyxml2::XMLElement* ScriptComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(ScriptComponent::g_CompId.c_str());

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

   // ExternalScriptObj
   if (m_isExternalScriptUsed) {
      tinyxml2::XMLElement* pExternalScriptObj = pDoc->NewElement("ExternalScriptObj");
      pExternalScriptObj->SetAttribute("varName", m_externalScriptObjVarName.c_str());
      pExternalScriptObj->SetAttribute("modulePath", m_externalScriptObjPath.c_str());
      pExternalScriptObj->SetAttribute("className", m_externalScriptObjClass.c_str());
      pBaseElement->LinkEndChild(pExternalScriptObj);
   }

   return pBaseElement;
}

pybind11::object ScriptComponent::GetObject()
{
   return m_pyObject;
}
} // namespace BIEngine
