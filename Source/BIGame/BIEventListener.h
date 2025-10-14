#pragma once

#include <glm/glm.hpp>

#include "PlayerManager/PlayerManager.h"
#include "../BIEngine/EventManager/EventManager.h"
#include "../BIEngine/Actors/Actor.h"

class EvtData_Player_Created : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   explicit EvtData_Player_Created(BIEngine::SharedPtr<Player> pPlayer)
      : m_pPlayer(pPlayer)
   {
   }

   virtual const BIEngine::EventType& GetEventType() const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy() const
   {
      return BIEngine::MakeShared<EvtData_Player_Created>(m_pPlayer);
   }

   virtual const char* GetName() const
   {
      return "EvtData_Player_Created";
   }

   BIEngine::SharedPtr<Player> GetPlayer() const { return m_pPlayer; }

private:
   BIEngine::SharedPtr<Player> m_pPlayer;
};

class EvtData_PlayerActor_Created : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   EvtData_PlayerActor_Created(int playerId, BIEngine::ActorId id)
      : m_playerId(playerId), m_actorId(id)
   {
   }

   virtual const BIEngine::EventType& GetEventType(void) const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy(void) const
   {
      return BIEngine::MakeShared<EvtData_PlayerActor_Created>(m_playerId, m_actorId);
   }

   virtual const char* GetName(void) const
   {
      return "EvtData_PlayerActor_Created";
   }

   int GetPlayerId(void) const { return m_playerId; }

   BIEngine::ActorId GetActorId(void) const { return m_actorId; }

private:
   int m_playerId;
   BIEngine::ActorId m_actorId = BIEngine::Actor::INVALID_ACTOR_ID;
};

class EvtData_Move : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   EvtData_Move()
      : m_playerId(PlayerManager::INVALID_PLAYER_ID), m_desiredHorizontalAmount(0.0f), m_desiredVerticalAmount(0.0f)
   {
   }

   EvtData_Move(PlayerId playerId, float desiredHorizontalAmount, float desiredVerticalAmount)
      : m_playerId(playerId), m_desiredHorizontalAmount(desiredHorizontalAmount), m_desiredVerticalAmount(desiredVerticalAmount)
   {
   }

   virtual const BIEngine::EventType& GetEventType(void) const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy(void) const
   {
      return BIEngine::MakeShared<EvtData_Move>(m_playerId, m_desiredHorizontalAmount, m_desiredVerticalAmount);
   }

   virtual const char* GetName(void) const
   {
      return "EvtData_Move";
   }

   virtual void Write(BIEngine::OutputMemoryBitStream& out) const override
   {
      Serialize(out, m_playerId);
      Serialize(out, m_desiredHorizontalAmount);
      Serialize(out, m_desiredVerticalAmount);
   }

   virtual void Read(BIEngine::InputMemoryBitStream& in) override
   {
      Deserialize(in, m_playerId);
      Deserialize(in, m_desiredHorizontalAmount);
      Deserialize(in, m_desiredVerticalAmount);
   }

   PlayerId GetPlayerId() const { return m_playerId; };

   float GetDesiredHorizontalAmount() const { return m_desiredHorizontalAmount; }

   float GetDesiredVerticalAmount() const { return m_desiredVerticalAmount; }

private:
   PlayerId m_playerId;

   float m_desiredHorizontalAmount = 0.0f;
   float m_desiredVerticalAmount = 0.0f;
};

class EvtData_Turn : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   EvtData_Turn() = default;

   EvtData_Turn(PlayerId playerId, const glm::vec2& desiredDir)
      : m_playerId(playerId), m_desiredDir(desiredDir)
   {
   }

   virtual const BIEngine::EventType& GetEventType(void) const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy(void) const
   {
      return BIEngine::MakeShared<EvtData_Turn>(m_playerId, m_desiredDir);
   }

   virtual const char* GetName(void) const
   {
      return "EvtData_Turn";
   }

   virtual void Write(BIEngine::OutputMemoryBitStream& out) const override
   {
      Serialize(out, m_playerId);
      out.WriteBytes(&m_desiredDir.x, sizeof(m_desiredDir));
   }

   virtual void Read(BIEngine::InputMemoryBitStream& in) override
   {
      Deserialize(in, m_playerId);
      in.ReadBytes(&m_desiredDir.x, sizeof(m_desiredDir));
   }

   PlayerId GetPlayerId() const { return m_playerId; };

   const glm::vec2& GetDesiredDir() const { return m_desiredDir; }

private:
   PlayerId m_playerId = PlayerManager::INVALID_PLAYER_ID;

   glm::vec2 m_desiredDir = glm::vec2(0.0f);
};

class EvtData_PrimaryAttack : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   EvtData_PrimaryAttack() = default;

   EvtData_PrimaryAttack(PlayerId playerId)
      : m_playerId(playerId)
   {
   }

   virtual const BIEngine::EventType& GetEventType(void) const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy(void) const
   {
      return BIEngine::MakeShared<EvtData_PrimaryAttack>(m_playerId);
   }

   virtual const char* GetName(void) const
   {
      return "EvtData_PrimaryAttack";
   }

   virtual void Write(BIEngine::OutputMemoryBitStream& out) const override
   {
      Serialize(out, m_playerId);
   }

   virtual void Read(BIEngine::InputMemoryBitStream& in) override
   {
      Deserialize(in, m_playerId);
   }

   PlayerId GetPlayerId() const { return m_playerId; };

private:
   PlayerId m_playerId = PlayerManager::INVALID_PLAYER_ID;
};

class EvtData_PlayerCommandMoveTo : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   EvtData_PlayerCommandMoveTo()
      : m_playerId(PlayerManager::INVALID_PLAYER_ID), m_pos()
   {
   }

   EvtData_PlayerCommandMoveTo(PlayerId playerId, const glm::vec3& pos)
      : m_playerId(playerId), m_pos(pos)
   {
   }

   virtual const BIEngine::EventType& GetEventType(void) const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy(void) const
   {
      return BIEngine::MakeShared<EvtData_PlayerCommandMoveTo>(m_playerId, m_pos);
   }

   virtual const char* GetName(void) const
   {
      return "EvtData_PlayerCommandMoveTo";
   }

   virtual void Write(BIEngine::OutputMemoryBitStream& out) const override
   {
      Serialize(out, m_playerId);
      Serialize(out, m_pos.x);
      Serialize(out, m_pos.y);
      Serialize(out, m_pos.z);
   }

   virtual void Read(BIEngine::InputMemoryBitStream& in) override
   {
      Deserialize(in, m_playerId);
      Deserialize(in, m_pos.x);
      Deserialize(in, m_pos.y);
      Deserialize(in, m_pos.z);
   }

   PlayerId GetPlayerId() const { return m_playerId; };

   const glm::vec3& GetPosToMove() const { return m_pos; }

private:
   PlayerId m_playerId;

   glm::vec3 m_pos;
};

void BIRegisterEvents();
