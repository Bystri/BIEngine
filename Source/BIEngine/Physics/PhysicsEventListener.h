#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "../EventManager/EventManager.h"
#include "../Scripting/ScriptEvent.h"
#include "../Actors/Actor.h"

namespace BIEngine
{

	//Данное событие возникает, когда физический объект входит в триггерную зону
	class EvtData_PhysTrigger_Enter : public BaseEventData
	{
		int m_triggerID;
		ActorId m_other;

	public:
		static const EventType sk_EventType;

		virtual const EventType& GetEventType() const
		{
			return sk_EventType;
		}

		EvtData_PhysTrigger_Enter()
		{
			m_triggerID = -1;
			m_other = Actor::INVALID_ACTOR_ID;
		}

		explicit EvtData_PhysTrigger_Enter(int triggerID, ActorId other)
			: m_triggerID(triggerID),
			m_other(other)
		{}

		IEventDataPtr Copy() const
		{
			return std::make_shared<EvtData_PhysTrigger_Enter>(m_triggerID, m_other);
		}

		virtual const char* GetName() const
		{
			return "EvtData_PhysTrigger_Enter";
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

	//Данное событие возникает, когда физический объект выходит из триггерной зоны
	class EvtData_PhysTrigger_Leave : public BaseEventData
	{
		int m_triggerID;
		ActorId m_other;

	public:
		static const EventType sk_EventType;

		virtual const EventType& GetEventType() const
		{
			return sk_EventType;
		}

		EvtData_PhysTrigger_Leave()
		{
			m_triggerID = -1;
			m_other = Actor::INVALID_ACTOR_ID;
		}

		explicit EvtData_PhysTrigger_Leave(int triggerID, ActorId other)
			: m_triggerID(triggerID),
			m_other(other)
		{}

		virtual IEventDataPtr Copy() const
		{
			return std::make_shared<EvtData_PhysTrigger_Leave>(m_triggerID, m_other);
		}

		virtual const char* GetName() const
		{
			return "EvtData_PhysTrigger_Leave";
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

	//Данное событие возникает, когда между физическими объектами возникает столкновение
	//Данное событие может быть получено внутри lua-скрипта
	class EvtData_PhysCollision : public ScriptEvent
	{
		ActorId m_ActorA;
		ActorId m_ActorB;
		glm::vec3 m_SumNormalForce;
		float m_friction;
		std::vector<glm::vec3> m_CollisionPointsA;
		std::vector<glm::vec3> m_CollisionPointsB;

	public:
		static const EventType sk_EventType;

		virtual const EventType& GetEventType() const
		{
			return sk_EventType;
		}

		EvtData_PhysCollision()
		{
			m_ActorA = Actor::INVALID_ACTOR_ID;
			m_ActorB = Actor::INVALID_ACTOR_ID;
			m_SumNormalForce = glm::vec3();
			m_friction = 0.0f;
		}

		explicit EvtData_PhysCollision(ActorId actorA,
			ActorId actorB,
			const glm::vec3& sumNormalForce,
			float friction,
			const std::vector<glm::vec3>& collisionPointsA,
			const std::vector<glm::vec3>& collisionPointsB)
			: m_ActorA(actorA),
			m_ActorB(actorB),
			m_SumNormalForce(sumNormalForce),
			m_friction(friction),
			m_CollisionPointsA(collisionPointsA),
			m_CollisionPointsB(collisionPointsB)
		{}

		virtual IEventDataPtr Copy() const
		{
			return std::make_shared<EvtData_PhysCollision>(m_ActorA, m_ActorB, m_SumNormalForce, m_friction, m_CollisionPointsA, m_CollisionPointsB);
		}

		virtual const char* GetName() const
		{
			return "EvtData_PhysCollision";
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

		const std::vector<glm::vec3>& GetCollisionPointsA() const
		{
			return m_CollisionPointsA;
		}

		const std::vector<glm::vec3>& GetCollisionPointsB() const
		{
			return m_CollisionPointsB;
		}

		virtual void BuildEventData();

		EXPORT_FOR_SCRIPT_EVENT(EvtData_PhysCollision);
	};

	//Данное событие возникает, когда прекращается столкновение между двумя физическими объектами
	class EvtData_PhysSeparation : public BaseEventData
	{
		ActorId m_ActorA;
		ActorId m_ActorB;

	public:
		static const EventType sk_EventType;

		virtual const EventType& GetEventType() const
		{
			return sk_EventType;
		}

		EvtData_PhysSeparation()
		{

			m_ActorA = Actor::INVALID_ACTOR_ID;
			m_ActorB = Actor::INVALID_ACTOR_ID;
		}

		explicit EvtData_PhysSeparation(ActorId actorA, ActorId actorB)
			: m_ActorA(actorA)
			, m_ActorB(actorB)
		{}

		virtual IEventDataPtr Copy() const
		{
			return std::make_shared<EvtData_PhysSeparation>(m_ActorA, m_ActorB);
		}

		virtual const char* GetName() const
		{
			return "EvtData_PhysSeparation";
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

}

