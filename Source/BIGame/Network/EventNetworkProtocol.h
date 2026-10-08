#pragma once

#include "../../../BIEngine/EventManager/EventManager.h"
#include "../../../BIEngine/StdLib/Algorithm.h"
#include "../BIEventListener.h"

struct CharacterMovementSnapshot {
   uint32_t lastProcessedSequence = 0;
   glm::vec3 position = glm::vec3(0.0f);
   glm::vec3 rotation = glm::vec3(0.0f);
   glm::vec3 velocity = glm::vec3(0.0f);
   glm::vec2 direction = glm::vec2(0.0f);
   glm::vec3 inputVelocity = glm::vec3(0.0f);
   glm::vec2 inputDirection = glm::vec2(0.0f);
   float orientation = 0.0f;
   float angularVelocity = 0.0f;
};

class EventProtocolLeader : public BIEngine::NetworkProtocol {
public:
   static const BIEngine::NetworkProtocolType sk_ProtocolType;

   EventProtocolLeader();
   virtual ~EventProtocolLeader();

   static EventProtocolLeader* Get() { return s_pInstance; }
   bool IsLocalPlayerActor(const BIEngine::Actor* pActor) const;

protected:
   virtual const BIEngine::NetworkProtocolType& GetType() const override { return sk_ProtocolType; }

   virtual void RegisterPeer(uint32_t peerId) override;
   virtual void UnregisterPeer(uint32_t peerId) override;

   virtual void ReceiveMessage(BIEngine::PeerId peerId, BIEngine::InputMemoryBitStream& inputStream) override;
   virtual void OnBeforePacketsSend(BIEngine::NetworkMessagesManager* pNetworkMessagesManager) override;

private:
   void NewPlayerActorDelegate(BIEngine::IEventDataPtr pEventData);
   void StoreEventToForwardDelegate(BIEngine::IEventDataPtr pEventData);

   void Reconcile(const CharacterMovementSnapshot& snapshot);

private:
   struct PendingEvent {
       uint32_t sequence;
       BIEngine::IEventDataPtr event;
   };

   BIEngine::DynamicArray<PendingEvent> m_eventsToSend;
   BIEngine::DynamicArray<PendingEvent> m_unacknowledgedEvents;
   BIEngine::DynamicArray<uint32_t> m_peersToSend;
   uint32_t m_nextSequence = 1;
   uint32_t m_lastAcknowledgedSequence = 0;

   BIEngine::EventManager::DelegateHandler m_storeEventCommandCharacterInputDelegateHandler;
   BIEngine::EventManager::DelegateHandler m_newPlayerActorDelegateHandler;

   BIEngine::WeakPtr<BIEngine::Actor> m_pLocalPlayerActor;
   static EventProtocolLeader* s_pInstance;
};

class EventProtocolFollower : public BIEngine::NetworkProtocol {
public:
   static const BIEngine::NetworkProtocolType sk_ProtocolType;

   static EventProtocolFollower* Get() { return s_pInstance; }
   EventProtocolFollower();
   virtual ~EventProtocolFollower();
   void BindPlayerActor(BIEngine::PeerId peerId, BIEngine::SharedPtr<BIEngine::Actor> actor);

   virtual const BIEngine::NetworkProtocolType& GetType() const override { return sk_ProtocolType; }

protected:
   virtual void RegisterPeer(uint32_t peerId) override;
   virtual void UnregisterPeer(uint32_t peerId) override;

   virtual void OnBeforePacketsSend(BIEngine::NetworkMessagesManager* pNetworkMessagesManager) override;
   virtual void ReceiveMessage(BIEngine::PeerId peerId, BIEngine::InputMemoryBitStream& inputStream) override;

private:
    struct PeerInfo
    {
        uint32_t peerId = -1;
        BIEngine::WeakPtr<BIEngine::Actor> playerActor;
    };

    BIEngine::DynamicArray<PeerInfo> m_peersToSend;
    static EventProtocolFollower* s_pInstance;
};
