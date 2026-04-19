#pragma once

#include "../BIGame/BIEventListener.h"

class EvtData_NetPeer_Connected : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   explicit EvtData_NetPeer_Connected(BIEngine::PeerId id)
      : m_peerId(id)
   {
   }

   virtual const BIEngine::EventType& GetEventType() const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy() const
   {
      return BIEngine::MakeShared<EvtData_NetPeer_Connected>(m_peerId);
   }

   virtual const char* GetName() const
   {
      return "EvtData_NetPeer_Connected";
   }

   BIEngine::PeerId GetPeerId() const { return m_peerId; }

private:
   BIEngine::PeerId m_peerId;
};

class EvtData_NetPeer_Disonnected : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   explicit EvtData_NetPeer_Disonnected(BIEngine::PeerId id)
      : m_peerId(id)
   {
   }

   virtual const BIEngine::EventType& GetEventType() const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy() const
   {
      return BIEngine::MakeShared<EvtData_NetPeer_Disonnected>(m_peerId);
   }

   virtual const char* GetName() const
   {
      return "EvtData_NetPeer_Disonnected";
   }

   BIEngine::PeerId GetPeerId() const { return m_peerId; }

private:
   BIEngine::PeerId m_peerId;
};

void BIRegisterServerEvents();