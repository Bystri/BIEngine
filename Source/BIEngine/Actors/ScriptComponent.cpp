
#include "ScriptComponent.h"
#include "../Scripting/LuaStateManager.h"
#include "../Utilities/String.h"

#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"

namespace BIEngine
{

    // Имя метатблицы, где будут жить все экспортированные функции в Lua-среде
    static const char* METATABLE_NAME = "ScriptComponentMetaTable";

    ComponentId ScriptComponent::g_CompId = "ScriptComponent";

    ScriptComponent::ScriptComponent()
    {
        m_scriptObject.AssignNil(LuaStateManager::Get()->GetLuaState());
        m_scriptDestructor.AssignNil(LuaStateManager::Get()->GetLuaState());
    }

    ScriptComponent::~ScriptComponent(void)
    {
        // Вызываем деструктор, если есть такой
        if (m_scriptDestructor.IsFunction())
        {
            LuaPlus::LuaFunction<void> func(m_scriptDestructor);
            func(m_scriptObject);
        }

        // Чистим обэект скрипта
        m_scriptObject.AssignNil(LuaStateManager::Get()->GetLuaState());
    }

    bool ScriptComponent::Init(tinyxml2::XMLElement* pData)
    {
        LuaStateManager* pStateMgr = LuaStateManager::Get();
        assert(pStateMgr);

        tinyxml2::XMLElement* pScriptObjectElement = pData->FirstChildElement("ScriptObject");
        if (!pScriptObjectElement)
        {
            Logger::WriteLog(Logger::LogType::ERROR, "No <ScriptObject> tag in XML");
            return false;
        }

        const char* temp = NULL;

        temp = pScriptObjectElement->Attribute("constructor");
        if (temp)
            m_constructorName = temp;

        temp = pScriptObjectElement->Attribute("destructor");
        if (temp)
            m_destructorName = temp;

        // Если у нас есть конструктор для объекта, то мы этот самый оъект и создаем
        if (!m_constructorName.empty())
        {
            m_scriptConstructor = pStateMgr->GetGlobalVars().Lookup(m_constructorName.c_str());
            if (m_scriptConstructor.IsFunction())
            {
                //Само создание объекта
                if (m_scriptObject.IsNil())
                {
                    m_scriptObject.AssignNewTable(pStateMgr->GetLuaState());
                    CreateScriptObject();
                }
            }
        }

        // Если есть, деструктор вызывается как функция, в которую передает скриптовый объект во время вызова деструктора компонента
        if (!m_destructorName.empty())
        {
            m_scriptDestructor = pStateMgr->GetGlobalVars().Lookup(m_destructorName.c_str());
        }

        // читаем данные из <ScriptData>
        tinyxml2::XMLElement* pScriptDataElement = pData->FirstChildElement("ScriptData");
        if (pScriptDataElement)
        {
            if (m_scriptObject.IsNil())
            {
                Logger::WriteLog(Logger::LogType::ERROR, "m_scriptObject cannot be nil when ScriptData has been defined");
                return false;
            }

            for (const tinyxml2::XMLAttribute* pAttribute = pScriptDataElement->FirstAttribute(); pAttribute != nullptr; pAttribute = pAttribute->Next())
            {
                m_scriptData.push_back({pAttribute->Name(), pAttribute->Value() });
                m_scriptObject.SetString(pAttribute->Name(), pAttribute->Value());
            }
        }

        return true;
    }

    void ScriptComponent::PostInit()
    {
        // call the script constructor if one exists
        if (m_scriptConstructor.IsFunction())
        {
            LuaPlus::LuaFunction<bool> func(m_scriptConstructor);
            func(m_scriptObject);
        }
    }

    tinyxml2::XMLElement* ScriptComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
    {
        tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

        // ScriptObject
        tinyxml2::XMLElement* pScriptObjectElement = pDoc->NewElement("ScriptObject");
 
        if (!m_constructorName.empty())
            pScriptObjectElement->SetAttribute("constructor", m_constructorName.c_str());
        if (!m_destructorName.empty())
            pScriptObjectElement->SetAttribute("destructor", m_destructorName.c_str());
        pBaseElement->LinkEndChild(pScriptObjectElement);

        // ScriptData
        tinyxml2::XMLElement* pScriptData = pDoc->NewElement("ScriptData");

        for (const auto& data : m_scriptData)
        {
            pScriptData->SetAttribute(data.first.c_str(), data.second.c_str());
        }
        pBaseElement->LinkEndChild(pScriptData);

        return pBaseElement;
    }

    void ScriptComponent::CreateScriptObject()
    {
        LuaStateManager* pStateMgr = LuaStateManager::Get();
        assert(pStateMgr);
        assert(!m_scriptObject.IsNil());

        LuaPlus::LuaObject metaTableObj = pStateMgr->GetGlobalVars().Lookup(METATABLE_NAME);
        assert(!metaTableObj.IsNil());

        LuaPlus::LuaObject boxedPtr = pStateMgr->GetLuaState()->BoxPointer(this);
        boxedPtr.SetMetaTable(metaTableObj);
        m_scriptObject.SetLightUserData("__object", this);
        m_scriptObject.SetMetaTable(metaTableObj);
    }

    void ScriptComponent::RegisterScriptFunctions(void)
    {
        LuaPlus::LuaObject metaTableObj = LuaStateManager::Get()->GetGlobalVars().CreateTable(METATABLE_NAME);
        metaTableObj.SetObject("__index", metaTableObj);

        metaTableObj.RegisterObjectDirect("GetActorId", (ScriptComponent*)0, &ScriptComponent::GetActorId);
        metaTableObj.RegisterObjectDirect("GetPos", (ScriptComponent*)0, &ScriptComponent::GetPos);
        metaTableObj.RegisterObjectDirect("SetPos", (ScriptComponent*)0, &ScriptComponent::SetPos);
        metaTableObj.RegisterObjectDirect("GetVelocity", (ScriptComponent*)0, &ScriptComponent::GetVelocity);
        metaTableObj.RegisterObjectDirect("GetOrientation", (ScriptComponent*)0, &ScriptComponent::GetOrientation);
        metaTableObj.RegisterObjectDirect("GetRotation", (ScriptComponent*)0, &ScriptComponent::GetRotation);
    }

    void ScriptComponent::UnregisterScriptFunctions()
    {
        LuaPlus::LuaObject metaTableObj = LuaStateManager::Get()->GetGlobalVars().Lookup(METATABLE_NAME);
        if (!metaTableObj.IsNil())
            metaTableObj.AssignNil(LuaStateManager::Get()->GetLuaState());
    }

    LuaPlus::LuaObject ScriptComponent::GetActorId()
    {
        LuaPlus::LuaObject ret;
        ret.AssignInteger(LuaStateManager::Get()->GetLuaState(), m_pOwner->GetId());
        return ret;
    }

    LuaPlus::LuaObject ScriptComponent::GetPos() const
    {
        LuaPlus::LuaObject ret;

        std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
        if (pTransformComponent)
            LuaStateManager::Get()->ConvertVec2ToTable(pTransformComponent->GetPosition(), ret);
        else
            ret.AssignNil(LuaStateManager::Get()->GetLuaState());

        return ret;
    }

    void ScriptComponent::SetPos(LuaPlus::LuaObject newPos)
    {
        std::shared_ptr<PhysicsComponent> pPhysicsComponent = m_pOwner->GetComponent<PhysicsComponent>(PhysicsComponent::g_CompId).lock();
        if (pPhysicsComponent)
        {
            glm::vec2 pos;
            LuaStateManager::Get()->ConvertTableToVec2(newPos, pos);
            pPhysicsComponent->SetPosition(pos.x, pos.y);
        }
        else
        {
            Logger::WriteLog(Logger::LogType::ERROR, "Attempting to call SetPos() on an actor with no physcial component; ActorId: " + std::to_string(m_pOwner->GetId()));
        }
    }

    LuaPlus::LuaObject ScriptComponent::GetVelocity() const
    {
        LuaPlus::LuaObject ret;

        std::shared_ptr<PhysicsComponent> pPhysicsComponent = m_pOwner->GetComponent<PhysicsComponent>(PhysicsComponent::g_CompId).lock();
        if (pPhysicsComponent)
            LuaStateManager::Get()->ConvertVec2ToTable(pPhysicsComponent->GetVelocity(), ret);
        else
            ret.AssignNil(LuaStateManager::Get()->GetLuaState());

        return ret;
    }

    float ScriptComponent::GetOrientation()
    {
        float angle = 0;

        std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
        if (pTransformComponent)
        {
            angle = pTransformComponent->GetRotation();
        }

        return angle;
    }

    float ScriptComponent::GetRotation()
    {
        float angle = 0;

        std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
        std::shared_ptr<PhysicsComponent> pPhysicsComponent = m_pOwner->GetComponent<PhysicsComponent>(PhysicsComponent::g_CompId).lock();
        if (pPhysicsComponent)
            angle = pPhysicsComponent->GetRotation();

        return angle;
    }
}