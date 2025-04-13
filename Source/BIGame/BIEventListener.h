#pragma once

#include "PlayerManager/PlayerManager.h"
#include "../BIEngine/EventManager/EventManager.h"
#include "../BIEngine/Actors/Actor.h"

class EvtData_Player_Created : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   explicit EvtData_Player_Created(std::shared_ptr<Player> pPlayer)
      : m_pPlayer(pPlayer)
   {
   }

   virtual const BIEngine::EventType& GetEventType() const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy() const
   {
      return std::make_shared<EvtData_Player_Created>(m_pPlayer);
   }

   virtual const char* GetName() const
   {
      return "EvtData_Player_Created";
   }

   std::shared_ptr<Player> GetPlayer() const { return m_pPlayer; }

private:
   std::shared_ptr<Player> m_pPlayer;
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
      return std::make_shared<EvtData_PlayerActor_Created>(m_playerId, m_actorId);
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
      return std::make_shared<EvtData_Move>(m_playerId, m_desiredHorizontalAmount, m_desiredVerticalAmount);
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

void BIRegisterEvents();
