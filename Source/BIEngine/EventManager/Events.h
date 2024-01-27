#pragma once

#include <glm/glm.hpp>

#include "EventManager.h"
#include "../Actors/Actor.h"
#include "../Graphics/SceneNodes.h"
#include "../UserInterface/IGameView.h"
#include "../Utilities/Logger.h"

namespace BIEngine {

// Данное событие отправляется, когда актер уничтожен
class EvtData_Actor_Created : public BaseEventData {
   ActorId m_id;

public:
   static const EventType sk_EventType;

   explicit EvtData_Actor_Created(ActorId id = Actor::INVALID_ACTOR_ID)
      : m_id(id)
   {
   }

   virtual const EventType& GetEventType(void) const
   {
      return sk_EventType;
   }

   virtual IEventDataPtr Copy(void) const
   {
      return std::make_shared<EvtData_Actor_Created>(m_id);
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
      return "EvtData_Actor_Created";
   }

   ActorId GetId(void) const { return m_id; }
};

// Данное событие отправляется, когда актер уничтожен
class EvtData_Destroy_Actor : public BaseEventData {
   ActorId m_id;

public:
   static const EventType sk_EventType;

   explicit EvtData_Destroy_Actor(ActorId id = Actor::INVALID_ACTOR_ID)
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

// Данное событие возникает, когда актер перемещается
class EvtData_Move_Actor : public BaseEventData {
   ActorId m_id;
   glm::vec3 m_postion;
   glm::vec3 m_rotation;

public:
   static const EventType sk_EventType;

   virtual const EventType& GetEventType() const
   {
      return sk_EventType;
   }

   EvtData_Move_Actor()
   {
      m_id = Actor::INVALID_ACTOR_ID;
   }

   EvtData_Move_Actor(ActorId id, const glm::vec3& pos, const glm::vec3& rot)
      : m_id(id), m_postion(pos), m_rotation(rot)
   {
   }

   virtual void Serialize(std::ostrstream& out) const
   {
      out << m_id << " " << m_postion.x << " " << m_postion.y << " " << m_postion.z << " " << m_rotation.x << " " << m_rotation.y << " " << m_rotation.z;
   }

   virtual void Deserialize(std::istrstream& in)
   {
      in >> m_id;
      in >> m_postion.x >> m_postion.y >> m_postion.z;
      in >> m_rotation.x >> m_rotation.y >> m_rotation.z;
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

   const glm::vec3& GetPosition() const
   {
      return m_postion;
   }

   const glm::vec3& GetRotation() const
   {
      return m_rotation;
   }
};

// Данное событие возникает, когда создается новый актер с компонентом, требующим отрисовку на экране
class EvtData_New_Render_Component : public BaseEventData {
   ActorId m_actorId;
   std::shared_ptr<SceneNode> m_pSceneNode;

public:
   static const EventType sk_EventType;

   EvtData_New_Render_Component()
   {
      m_actorId = Actor::INVALID_ACTOR_ID;
   }

   EvtData_New_Render_Component(ActorId actorId, std::shared_ptr<SceneNode> pSceneNode)
      : m_actorId(actorId), m_pSceneNode(pSceneNode)
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

// Событие является запросом на уничтожение актера
// TODO: а зачем уничтожение актера идет через событие? Не проще сразу предоставить прямой вызов к GameLogic::DestroyActor в скрипт? Как вообще это сделано в Unity?
class EvtData_Request_Destroy_Actor : public BaseEventData {
   ActorId m_actorId;

public:
   static const EventType sk_EventType;

   EvtData_Request_Destroy_Actor()
   {
      m_actorId = Actor::INVALID_ACTOR_ID;
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
};

} // namespace BIEngine
