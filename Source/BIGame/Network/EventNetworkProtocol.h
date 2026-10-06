#pragma once

#include "../../../BIEngine/EventManager/EventManager.h"
#include "../../../BIEngine/StdLib/Algorithm.h"
#include "../BIEventListener.h"

class EventProtocolLeader : public BIEngine::NetworkProtocol {
public:
   static const BIEngine::NetworkProtocolType sk_ProtocolType;

   EventProtocolLeader();
   virtual ~EventProtocolLeader();

protected:
   virtual const BIEngine::NetworkProtocolType& GetType() const override { return sk_ProtocolType; }

   virtual void RegisterPeer(uint32_t peerId) override;
   virtual void UnregisterPeer(uint32_t peerId) override;

   virtual void OnBeforePacketsSend(BIEngine::NetworkMessagesManager* pNetworkMessagesManager) override;

private:
   void StoreEventToForwardDelegate(BIEngine::IEventDataPtr pEventData);

private:
   BIEngine::DynamicArray<BIEngine::IEventDataPtr> m_eventsToSend;
   BIEngine::DynamicArray<uint32_t> m_peersToSend;

   BIEngine::EventManager::DelegateHandler m_storeEventCommandCharacterInputDelegateHandler;
};

class EventProtocolFollower : public BIEngine::NetworkProtocol {
public:
   static const BIEngine::NetworkProtocolType sk_ProtocolType;

   virtual const BIEngine::NetworkProtocolType& GetType() const override { return sk_ProtocolType; }

protected:
   virtual void ReceiveMessage(BIEngine::PeerId peerId, BIEngine::InputMemoryBitStream& inputStream) override;
};
