#pragma once

#include <glm/glm.hpp>

#include "EventManager.h"
#include "../Scripting/ScriptEvent.h"
#include "../Actors/Actor.h"
#include "../Graphics2D/SceneNodes.h"
#include "../UserInterface/IGameView.h"
#include "../Utilities/Logger.h"

namespace BIEngine
{
    //Регестрирует события, которые могут быть посла/обработы в среде выполнения скриптов
    void RegisterEngineScriptEvents();


    //Данное событие отправляется, когда актер уничтожен
    class EvtData_Destroy_Actor : public BaseEventData
    {
        ActorId m_id;

    public:
        static const EventType sk_EventType;

        explicit EvtData_Destroy_Actor(ActorId id = Actor::INBALID_ACTOR_ID)
            : m_id(id)
        {
        }

        virtual const EventType& GetEventType(void) const
        {
            return sk_EventType;
        }

        virtual IEventDataPtr Copy(void) const
        {
            return std::make_shared<EvtData_Destroy_Actor>(m_id);
        }

        virtual void Serialize(std::ostrstream& out) const
        {
            out << m_id;
        }

        virtual void Deserialize(std::istrstream& in)
        {
            in >> m_id;
        }

        virtual const char* GetName(void) const
        {
            return "EvtData_Destroy_Actor";
        }

        ActorId GetId(void) const { return m_id; }
    };

    //Данное событие возникает, когда актер перемещается
    class EvtData_Move_Actor : public BaseEventData
    {
        ActorId m_id;
        glm::vec2 m_postion;
        float m_rotation;

    public:
        static const EventType sk_EventType;

        virtual const EventType& GetEventType() const
        {
            return sk_EventType;
        }

        EvtData_Move_Actor()
        {
            m_id = Actor::INBALID_ACTOR_ID;
        }

        EvtData_Move_Actor(ActorId id, const glm::vec2& pos, float rot)
            : m_id(id), m_postion(pos), m_rotation(rot)
        {
        }

        virtual void Serialize(std::ostrstream& out) const
        {
            out << m_id << " " << m_postion.x << " " << m_postion.y << " " << m_rotation;
        }

        virtual void Deserialize(std::istrstream& in)
        {
            in >> m_id;
            in >> m_postion.x >> m_postion.y;
            in >> m_rotation;
        }

        virtual IEventDataPtr Copy() const
        {
            return std::make_shared<EvtData_Move_Actor>(m_id, m_postion, m_rotation);
        }

        virtual const char* GetName() const
        {
            return "EvtData_Move_Actor";
        }

        ActorId GetId() const
        {
            return m_id;
        }

        const glm::vec2& GetPosition() const
        {
            return m_postion;
        }

        float GetRotation() const
        {
            return m_rotation;
        }
    };

    //Данное событие возникает, когда создается новый актер с компонентом, требующим отрисовку на экране
    class EvtData_New_Render_Component : public BaseEventData
    {
        ActorId m_actorId;
        std::shared_ptr<SceneNode> m_pSceneNode;

    public:
        static const EventType sk_EventType;

        EvtData_New_Render_Component()
        {
            m_actorId = Actor::INBALID_ACTOR_ID;
        }

        EvtData_New_Render_Component(ActorId actorId, std::shared_ptr<SceneNode> pSceneNode)
            : m_actorId(actorId)
            , m_pSceneNode(pSceneNode)
        {
        }

        virtual void Serialize(std::ostrstream& out) const
        {
            Logger::WriteLog(Logger::LogType::ERROR, "EvtData_New_Render_Component should not be serialzied!");
        }

        virtual void Deserialize(std::istrstream& in)
        {
            Logger::WriteLog(Logger::LogType::ERROR, "EvtData_New_Render_Component should not be serialzied!");
        }

        virtual const EventType& GetEventType() const
        {
            return sk_EventType;
        }

        virtual IEventDataPtr Copy() const
        {
            return std::make_shared<EvtData_New_Render_Component>(m_actorId, m_pSceneNode);
        }

        virtual const char* GetName() const
        {
            return "EvtData_New_Render_Component";
        }

        const ActorId GetActorId() const
        {
            return m_actorId;
        }

        std::shared_ptr<SceneNode> GetSceneNode() const
        {
            return m_pSceneNode;
        }
    };


    //Данное событие возникает, когда создается новый актер с компонентом, являющимся камерой
    class EvtData_New_Camera_Component : public BaseEventData
    {
        std::shared_ptr<Actor> m_pCameraActor;

    public:
        static const EventType sk_EventType;

        EvtData_New_Camera_Component()
            : m_pCameraActor(nullptr)
        {
        }

        EvtData_New_Camera_Component(std::shared_ptr<Actor> pCameraActor)
            : m_pCameraActor(pCameraActor)
        {
        }

        virtual void Serialize(std::ostrstream& out) const
        {
            Logger::WriteLog(Logger::LogType::ERROR, "EvtData_New_Camera_Component should not be serialzied!");
        }

        virtual void Deserialize(std::istrstream& in)
        {
            Logger::WriteLog(Logger::LogType::ERROR, "EvtData_New_Camera_Component should not be serialzied!");
        }

        virtual const EventType& GetEventType() const
        {
            return sk_EventType;
        }

        virtual IEventDataPtr Copy() const
        {
            return std::make_shared<EvtData_New_Camera_Component>(m_pCameraActor);
        }

        virtual const char* GetName() const
        {
            return "EvtData_New_Camera_Component";
        }

        std::shared_ptr<Actor> GetCameraActor() const
        {
            return m_pCameraActor;
        }
    };

    //Событие является запросом на уничтожение актера
    //TODO: а зачем уничтожение актера идет через событие? Не проще сразу предоставить прямой вызов к GameLogic::DestroyActor в скрипт? Как вообще это сделано в Unity?
    class EvtData_Request_Destroy_Actor : public ScriptEvent
    {
        ActorId m_actorId;

    public:
        static const EventType sk_EventType;

        EvtData_Request_Destroy_Actor()
        {
            m_actorId = Actor::INBALID_ACTOR_ID;
        }

        EvtData_Request_Destroy_Actor(ActorId actorId)
        {
            m_actorId = actorId;
        }

        virtual const EventType& GetEventType() const
        {
            return sk_EventType;
        }

        virtual void Deserialize(std::istrstream& in)
        {
            in >> m_actorId;
        }

        virtual IEventDataPtr Copy() const
        {
            return std::make_shared<EvtData_Request_Destroy_Actor>(m_actorId);
        }

        virtual void Serialize(std::ostrstream& out) const
        {
            out << m_actorId;
        }

        virtual const char* GetName(void) const
        {
            return "EvtData_Request_Destroy_Actor";
        }

        ActorId GetActorId(void) const
        {
            return m_actorId;
        }

        virtual bool BuildEventFromScript(void)
        {
            if (m_eventData.IsInteger())
            {
                m_actorId = m_eventData.GetInteger();
                return true;
            }
            return false;
        }

        EXPORT_FOR_SCRIPT_EVENT(EvtData_Request_Destroy_Actor);
    };

}

