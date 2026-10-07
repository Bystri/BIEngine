#include "EventNetworkProtocol.h"

#include "../../BIEngine/Actors/TransformComponent.h"
#include "../../BIEngine/Actors/Physics3DComponent.h"
#include "../../BIEngine/EngineCore/GameApp.h"

const BIEngine::NetworkProtocolType EventProtocolLeader::sk_ProtocolType('EVNT');
const BIEngine::NetworkProtocolType EventProtocolFollower::sk_ProtocolType('EVNT');

/***EventProtocolWriter***/

EventProtocolLeader::EventProtocolLeader()
{
   m_newPlayerActorDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(EventProtocolLeader::NewPlayerActorDelegate), EvtData_PlayerActor_Created::sk_EventType);
   m_storeEventCommandCharacterInputDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(EventProtocolLeader::StoreEventToForwardDelegate), EvtData_CharacterInput::sk_EventType);
}

EventProtocolLeader::~EventProtocolLeader()
{
   BIEngine::EventManager::Get()->RemoveListener(m_storeEventCommandCharacterInputDelegateHandler);
   BIEngine::EventManager::Get()->RemoveListener(m_newPlayerActorDelegateHandler);
}

void EventProtocolLeader::RegisterPeer(uint32_t peerId)
{
   m_peersToSend.PushBack(peerId);
}

void EventProtocolLeader::UnregisterPeer(uint32_t peerId)
{
   const auto itr = BIEngine::Find(m_peersToSend.Begin(), m_peersToSend.End(), peerId);
   if (itr == m_peersToSend.End()) {
      return;
   }

   m_peersToSend.Erase(itr);
   if (m_peersToSend.Empty()) {
      m_eventsToSend.Clear();
      m_unacknowledgedEvents.Clear();
      m_lastAcknowledgedSequence = 0;
      m_nextSequence = 1;
   }
}

void EventProtocolLeader::ReceiveMessage(BIEngine::PeerId peerId, BIEngine::InputMemoryBitStream& inputStream)
{
    BIEngine::Deserialize(inputStream, m_lastAcknowledgedSequence);

    while (!m_unacknowledgedEvents.Empty() &&
        m_unacknowledgedEvents.Front().sequence <= m_lastAcknowledgedSequence) {
        m_unacknowledgedEvents.Erase(m_unacknowledgedEvents.Begin());
    }

    Reconcile();
}

void EventProtocolLeader::OnBeforePacketsSend(BIEngine::NetworkMessagesManager* pNetworkMessagesManager)
{
   if (m_eventsToSend.Empty()) {
      return;
   }

   for (size_t begin = 0; begin < m_eventsToSend.Size();) {
      const uint8_t count = static_cast<uint8_t>(
         m_eventsToSend.Size() - begin > 255 ? 255 : m_eventsToSend.Size() - begin);
      BIEngine::OutputMemoryBitStream eventPacket;
      BIEngine::Serialize(eventPacket, count);

      for (size_t i = begin; i < begin + count; ++i) {
         const auto& pending = m_eventsToSend[i];
         BIEngine::Serialize(eventPacket, pending.sequence);
         BIEngine::Serialize(eventPacket, pending.event->GetEventType());
         pending.event->Write(eventPacket);
      }

      for (auto& pPeer : m_peersToSend) {
         pNetworkMessagesManager->SendNetworkMessage(pPeer, GetType(), eventPacket);
      }
      begin += count;
   }

   m_eventsToSend.Clear();
}

void EventProtocolLeader::NewPlayerActorDelegate(BIEngine::IEventDataPtr pEventData)
{
   BIEngine::SharedPtr<EvtData_PlayerActor_Created> pCastEventData = BIEngine::StaticPointerCast<EvtData_PlayerActor_Created>(pEventData);

   if (pCastEventData->GetPlayerId() != PlayerManager::Get()->GetLocalPlayerId()) {
      return;
   }

   m_pLocalPlayerActor = BIEngine::g_pApp->m_pGameLogic->GetActor(pCastEventData->GetActorId());
}

void EventProtocolLeader::StoreEventToForwardDelegate(BIEngine::IEventDataPtr pEventData)
{
   if (m_peersToSend.Empty()) {
      return;
   }

   for (const auto& pending : m_unacknowledgedEvents) {
      if (pending.event == pEventData) {
         return;
      }
   }

   const uint32_t sequence = m_nextSequence++;
   PendingEvent pending{sequence, pEventData};
   m_eventsToSend.PushBack(pending);
   m_unacknowledgedEvents.PushBack(std::move(pending));
}

void EventProtocolLeader::Reconcile()
{
   auto actor = m_pLocalPlayerActor.Lock();
   if (!actor) {
      return;
   }

   auto transform = actor->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).Lock();
   if (!transform) {
      return;
   }

   BIEngine::g_pApp->m_pGameLogic->GetGamePhysics3D()->SetPosition(actor->GetId(), transform->GetPosition());
   auto physics = actor->GetComponent<BIEngine::Physics3DComponent>(BIEngine::Physics3DComponent::g_CompId).Lock();
   if (physics) {
      physics->Translate(glm::vec3(0.0f), transform->GetRotation());
   }

   const float fixedDt = 1.0f / BIEngine::g_pApp->m_options.fixedFps;
   for (const auto& pending : m_unacknowledgedEvents) {
      BIEngine::EventManager::Get()->TriggerEvent(pending.event);
      actor->OnFixedUpdate(fixedDt);
   }
}

/***EventProtocolReader***/

void EventProtocolFollower::RegisterPeer(uint32_t peerId)
{
    m_peersToSend.PushBack(PeerInfo{ peerId, 0 });
}

void EventProtocolFollower::UnregisterPeer(uint32_t peerId)
{
    const auto itr = BIEngine::FindIf(m_peersToSend.Begin(), m_peersToSend.End(), [peerId](const PeerInfo& info) {return info.peerId == peerId;});
    if (itr == m_peersToSend.End()) {
        return;
    }

    m_peersToSend.Erase(itr);
}

void EventProtocolFollower::OnBeforePacketsSend(BIEngine::NetworkMessagesManager* pNetworkMessagesManager)
{
    for (auto& pPeer : m_peersToSend) {
        BIEngine::OutputMemoryBitStream packet;
        BIEngine::Serialize(packet, pPeer.lastReceivedInputSequence);

        pNetworkMessagesManager->SendNetworkMessage(pPeer.peerId, GetType(), packet);
    }
}

void EventProtocolFollower::ReceiveMessage(BIEngine::PeerId peerId, BIEngine::InputMemoryBitStream& inputStream)
{
    const auto itr = BIEngine::FindIf(m_peersToSend.Begin(), m_peersToSend.End(), [peerId](const PeerInfo& info) {return info.peerId == peerId;});
    if (itr == m_peersToSend.End()) {
        return;
    }

   uint8_t eventCount = 0;
   BIEngine::Deserialize(inputStream, eventCount);

   while (eventCount > 0) {
      BIEngine::Deserialize(inputStream, itr->lastReceivedInputSequence);
      BIEngine::EventType eventType;
      BIEngine::Deserialize(inputStream, eventType);

      BIEngine::IEventDataPtr pEvent = BIEngine::g_eventFactory.Create(eventType);
      pEvent->Read(inputStream);

      BIEngine::EventManager::Get()->QueueEvent(pEvent);

      --eventCount;
   }
}
