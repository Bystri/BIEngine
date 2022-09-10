#pragma once

#include <LuaPlus/LuaPlus.h>

#include "../BIEngine/Scripting/ScriptEvent.h"
#include "../BIEngine/Utilities/Logger.h"
#include "../BIEngine/Actors/Actor.h"

using namespace BIEngine;

class EvtData_StartPaddleMove : public ScriptEvent
{
public:
    static const EventType sk_EventType;
    virtual const EventType& GetEventType() const { return sk_EventType; }

    EvtData_StartPaddleMove() : m_id(Actor::INBALID_ACTOR_ID), m_velocity(0) {}

    EvtData_StartPaddleMove(ActorId id, float acceleration) : m_id(id), m_velocity(acceleration) {}

    virtual IEventDataPtr Copy() const
    {
        return std::make_shared<EvtData_StartPaddleMove>(m_id, m_velocity);
    }

    virtual void Serialize(std::ostrstream& out) const
    {
        out << m_id << " ";
        out << m_velocity << " ";
    }

    virtual void Deserialize(std::istrstream& in)
    {
        in >> m_id;
        in >> m_velocity;
    }

    virtual const char* GetName() const
    {
        return "EvtData_StartSteer";
    }

    ActorId GetActorId() const
    {
        return m_id;
    }

    float GetVelocity() const
    {
        return m_velocity;
    }

    void Set(ActorId id)
    {
        m_id = id;
    }

    virtual bool BuildEventFromScript()
    {
        if (m_eventData.IsTable())
        {
            // ID
            LuaPlus::LuaObject temp = m_eventData.GetByName("id");
            if (temp.IsInteger())
            {
                m_id = temp.GetInteger();
            }
            else
            {
                Logger::WriteLog(Logger::LogType::ERROR, "Invalid id sent to EvtData_StartSteer; type = " + std::string(temp.TypeName()));
                return false;
            }

            temp = m_eventData.GetByName("velocity");
            if (temp.IsNumber())
                m_velocity = temp.GetFloat();
            else
                m_velocity = 80.0f;

            return true;
        }

        return false;
    }

    EXPORT_FOR_SCRIPT_EVENT(EvtData_StartPaddleMove);

private:
    ActorId m_id;
    float m_velocity;
};


class EvtData_EndPaddleMove : public ScriptEvent
{
public:
    static const EventType sk_EventType;
    virtual const EventType& GetEventType() const
    {
        return sk_EventType;
    }

    EvtData_EndPaddleMove() : m_id(Actor::INBALID_ACTOR_ID) {}

    EvtData_EndPaddleMove(ActorId id) : m_id(id) {}

    virtual IEventDataPtr Copy() const
    {
        return std::make_shared<EvtData_EndPaddleMove>(m_id);
    }

    virtual void Serialize(std::ostrstream& out) const
    {
        out << m_id << " ";
    }

    virtual void Deserialize(std::istrstream& in)
    {
        in >> m_id;
    }

    virtual const char* GetName() const
    {
        return "EvtData_EndThrust";
    }

    ActorId GetActorId() const
    {
        return m_id;
    }

    void Set(ActorId id)
    {
        m_id = id;
    }

    virtual bool BuildEventFromScript()
    {
        if (m_eventData.IsTable())
        {
            // ID
            LuaPlus::LuaObject temp = m_eventData.GetByName("id");
            if (temp.IsInteger())
            {
                m_id = temp.GetInteger();
            }
            else
            {
                Logger::WriteLog(Logger::LogType::ERROR, "Invalid id sent to EvtData_EndSteer; type = " + std::string(temp.TypeName()));
                return false;
            }

            return true;
        }

        return false;
    }

    EXPORT_FOR_SCRIPT_EVENT(EvtData_EndPaddleMove);

private:
    ActorId m_id;
};

class EvtData_UpdateUI : public ScriptEvent
{
public:
    static const EventType sk_EventType;
    virtual const EventType& GetEventType() const
    {
        return sk_EventType;
    }

    EvtData_UpdateUI() : m_player1Points(0), m_player2Points(0) {}

    EvtData_UpdateUI(unsigned int player1Points, unsigned int player2Points) : m_player1Points(player1Points), m_player2Points(player2Points) {}

    virtual IEventDataPtr Copy() const
    {
        return std::make_shared<EvtData_UpdateUI>(m_player1Points, m_player2Points);
    }

    virtual void Serialize(std::ostrstream& out) const
    {
        out << m_player1Points << " " << m_player2Points << " ";
    }

    virtual void Deserialize(std::istrstream& in)
    {
        in >> m_player1Points >> m_player2Points;
    }

    virtual const char* GetName() const
    {
        return "EvtData_UpdateUI";
    }

    unsigned int GetPlayer1Points() const
    {
        return m_player1Points;
    }

    unsigned int GetPlayer2Points() const
    {
        return m_player2Points;
    }

    void Set(unsigned int player1Points, unsigned int player2Points)
    {
        m_player1Points = player1Points;
        m_player2Points = player2Points;
    }

    virtual bool BuildEventFromScript()
    {
        if (m_eventData.IsTable())
        {
 
            LuaPlus::LuaObject p1Points = m_eventData.GetByName("player1Points");
            if (p1Points.IsInteger())
            {
                m_player1Points = p1Points.GetInteger();
            }
            else
            {
                Logger::WriteLog(Logger::LogType::ERROR, "Invalid id sent to EvtData_EndSteer; type = " + std::string(p1Points.TypeName()));
                return false;
            }

            LuaPlus::LuaObject p2Points = m_eventData.GetByName("player2Points");
            if (p2Points.IsInteger())
            {
                m_player1Points = p2Points.GetInteger();
            }
            else
            {
                Logger::WriteLog(Logger::LogType::ERROR, "Invalid id sent to EvtData_EndSteer; type = " + std::string(p2Points.TypeName()));
                return false;
            }

            return true;
        }

        return false;
    }

    EXPORT_FOR_SCRIPT_EVENT(EvtData_EndPaddleMove);

private:
    unsigned int m_player1Points;
    unsigned int m_player2Points;
};