#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "../EventManager/EventManager.h"
#include "../Actors/Actor.h"

namespace BIEngine {

// Данное событие возникает, когда физический объект входит в триггерную зону
class EvtData_Phys3DTrigger_Enter : public BaseEventData {
   int m_triggerID;
   ActorId m_other;

public:
   static const EventType sk_EventType;

   virtual const EventType& GetEventType() const
   {
      return sk_EventType;
   }

   EvtData_Phys3DTrigger_Enter()
   {
      m_triggerID = -1;
      m_other = Actor::INVALID_ACTOR_ID;
   }

   explicit EvtData_Phys3DTrigger_Enter(int triggerID, ActorId other)
      : m_triggerID(triggerID),
        m_other(other)
   {
   }

   IEventDataPtr Copy() const
   {
      return std::make_shared<EvtData_Phys3DTrigger_Enter>(m_triggerID, m_other);
   }

   virtual const char* GetName() const
   {
      return "EvtData_Phys3DTrigger_Enter";
   }

   int GetTriggerId() const
   {
      return m_triggerID;
   }

   ActorId GetOtherActor() const
   {
      return m_other;
   }
};

// Данное событие возникает, когда физический объект выходит из триггерной зоны
class EvtData_Phys3DTrigger_Leave : public BaseEventData {
   int m_triggerID;
   ActorId m_other;

public:
   static const EventType sk_EventType;

   virtual const EventType& GetEventType() const
   {
      return sk_EventType;
   }

   EvtData_Phys3DTrigger_Leave()
   {
      m_triggerID = -1;
      m_other = Actor::INVALID_ACTOR_ID;
   }

   explicit EvtData_Phys3DTrigger_Leave(int triggerID, ActorId other)
      : m_triggerID(triggerID),
        m_other(other)
   {
   }

   virtual IEventDataPtr Copy() const
   {
      return std::make_shared<EvtData_Phys3DTrigger_Leave>(m_triggerID, m_other);
   }

   virtual const char* GetName() const
   {
      return "EvtData_Phys3DTrigger_Leave";
   }

   int GetTriggerId() const
   {
      return m_triggerID;
   }

   ActorId GetOtherActor() const
   {
      return m_other;
   }
};

// Данное событие возникает, когда между физическими объектами возникает столкновение
// Данное событие может быть получено внутри Python-скрипта
class EvtData_Phys3DCollision : public BaseEventData {
   ActorId m_ActorA;
   ActorId m_ActorB;
   glm::vec3 m_SumNormalForce;
   float m_friction;
   std::vector<glm::vec3> m_CollisionPoints;

public:
   static const EventType sk_EventType;

   virtual const EventType& GetEventType() const
   {
      return sk_EventType;
   }

   EvtData_Phys3DCollision()
   {
      m_ActorA = Actor::INVALID_ACTOR_ID;
      m_ActorB = Actor::INVALID_ACTOR_ID;
      m_SumNormalForce = glm::vec3();
      m_friction = 0.0f;
   }

   explicit EvtData_Phys3DCollision(ActorId actorA, ActorId actorB, const glm::vec3& sumNormalForce, float friction, const std::vector<glm::vec3>& collisionPoints)
      : m_ActorA(actorA),
        m_ActorB(actorB),
        m_SumNormalForce(sumNormalForce),
        m_friction(friction),
        m_CollisionPoints(collisionPoints)
   {
   }

   virtual IEventDataPtr Copy() const
   {
      return std::make_shared<EvtData_Phys3DCollision>(m_ActorA, m_ActorB, m_SumNormalForce, m_friction, m_CollisionPoints);
   }

   virtual const char* GetName() const
   {
      return "EvtData_Phys3DCollision";
   }

   ActorId GetActorA() const
   {
      return m_ActorA;
   }

   ActorId GetActorB() const
   {
      return m_ActorB;
   }

   const glm::vec2& GetSumNormalForce() const
   {
      return m_SumNormalForce;
   }

   float GetSumFrictionForce() const
   {
      return m_friction;
   }

   const std::vector<glm::vec3>& GetCollisionPoints() const
   {
      return m_CollisionPoints;
   }
};

// Данное событие возникает, когда прекращается столкновение между двумя физическими объектами
class EvtData_Phys3DSeparation : public BaseEventData {
   ActorId m_ActorA;
   ActorId m_ActorB;

public:
   static const EventType sk_EventType;

   virtual const EventType& GetEventType() const
   {
      return sk_EventType;
   }

   EvtData_Phys3DSeparation()
   {

      m_ActorA = Actor::INVALID_ACTOR_ID;
      m_ActorB = Actor::INVALID_ACTOR_ID;
   }

   explicit EvtData_Phys3DSeparation(ActorId actorA, ActorId actorB)
      : m_ActorA(actorA), m_ActorB(actorB)
   {
   }

   virtual IEventDataPtr Copy() const
   {
      return std::make_shared<EvtData_Phys3DSeparation>(m_ActorA, m_ActorB);
   }

   virtual const char* GetName() const
   {
      return "EvtData_Phys3DSeparation";
   }

   ActorId GetActorA() const
   {
      return m_ActorA;
   }

   ActorId GetActorB() const
   {
      return m_ActorB;
   }
};

} // namespace BIEngine
