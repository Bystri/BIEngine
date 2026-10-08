#include "EventNetworkProtocol.h"

#include "../../BIEngine/Actors/TransformComponent.h"
#include "../../BIEngine/EngineCore/GameApp.h"
#include "../../BIEngine/Actors/PlayerComponent.h"
#include "../PlayerCommandBinderComponent.h"
#include "../Locomotion/LocomotionInfoComponent.h"
#include "../Movement/CharacterMovementReconciler.h"

const BIEngine::NetworkProtocolType EventProtocolLeader::sk_ProtocolType('EVNT');
const BIEngine::NetworkProtocolType EventProtocolFollower::sk_ProtocolType('EVNT');
EventProtocolLeader* EventProtocolLeader::s_pInstance = nullptr;
EventProtocolFollower* EventProtocolFollower::s_pInstance = nullptr;

/***EventProtocolWriter***/

EventProtocolLeader::EventProtocolLeader()
{
   BIEngine::Assert(s_pInstance == nullptr, "Only one event leader can exist");
   s_pInstance = this;
   m_newPlayerActorDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(EventProtocolLeader::NewPlayerActorDelegate), EvtData_PlayerActor_Created::sk_EventType);
   m_storeEventCommandCharacterInputDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(EventProtocolLeader::StoreEventToForwardDelegate), EvtData_CharacterInput::sk_EventType);
}

EventProtocolLeader::~EventProtocolLeader()
{
   BIEngine::EventManager::Get()->RemoveListener(m_storeEventCommandCharacterInputDelegateHandler);
   BIEngine::EventManager::Get()->RemoveListener(m_newPlayerActorDelegateHandler);
   s_pInstance = nullptr;
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
      m_eventHistory.Clear();
      m_inputHistory.Clear();
      m_nextSequence = 1;
   }
}

void EventProtocolLeader::ReceiveMessage(BIEngine::PeerId peerId, BIEngine::InputMemoryBitStream& inputStream)
{
   CharacterMovementSnapshot snapshot;
   BIEngine::Deserialize(inputStream, snapshot.lastProcessedSequence);
   BIEngine::Deserialize(inputStream, snapshot.state.position);
   BIEngine::Deserialize(inputStream, snapshot.state.rotation);
   BIEngine::Deserialize(inputStream, snapshot.state.velocity);
   BIEngine::Deserialize(inputStream, snapshot.state.direction.x);
   BIEngine::Deserialize(inputStream, snapshot.state.direction.y);
   BIEngine::Deserialize(inputStream, snapshot.state.inputVelocity);
   BIEngine::Deserialize(inputStream, snapshot.state.inputDirection.x);
   BIEngine::Deserialize(inputStream, snapshot.state.inputDirection.y);
   BIEngine::Deserialize(inputStream, snapshot.state.orientation);
   BIEngine::Deserialize(inputStream, snapshot.state.angularVelocity);
   if (!inputStream.HasReadError()) {
      auto actor = m_pLocalPlayerActor.Lock();
      if (actor) {
         const float fixedDt = 1.0f / BIEngine::g_pApp->m_options.fixedFps;
         if (CharacterMovementReconciler::Reconcile(*actor, snapshot, m_inputHistory, fixedDt)) {
            m_eventHistory.Acknowledge(snapshot.lastProcessedSequence);
         }
      }
   }
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

   if (pEventData->GetEventType() == EvtData_CharacterInput::sk_EventType &&
       BIEngine::StaticPointerCast<EvtData_CharacterInput>(pEventData)->GetPlayerId() == PlayerManager::INVALID_PLAYER_ID) {
      return;
   }

   const uint32_t sequence = m_nextSequence++;
   if (pEventData->GetEventType() == EvtData_CharacterInput::sk_EventType) {
      auto input = BIEngine::StaticPointerCast<EvtData_CharacterInput>(pEventData);
      input->SetSequence(sequence);
      CharacterInputCommand command;
      command.sequence = sequence;
      command.inputVelocity = glm::vec3(input->GetDesiredHorizontalAmount(), 0.0f, input->GetDesiredVerticalAmount());
      command.inputDirection = input->GetDesiredDir();
      m_inputHistory.Record(command);
   }
   PendingEvent pending{sequence, pEventData};
   m_eventsToSend.PushBack(pending);
   m_eventHistory.Record(sequence, pEventData);
}

bool EventProtocolLeader::IsLocalPlayerActor(const BIEngine::Actor* actor) const
{
   auto localActor = m_pLocalPlayerActor.Lock();
   return localActor && localActor.Get() == actor;
}

/***EventProtocolReader***/

EventProtocolFollower::EventProtocolFollower()
{
   BIEngine::Assert(s_pInstance == nullptr, "Only one event follower can exist");
   s_pInstance = this;
}

EventProtocolFollower::~EventProtocolFollower()
{
   s_pInstance = nullptr;
}

void EventProtocolFollower::BindPlayerActor(BIEngine::PeerId peerId, BIEngine::SharedPtr<BIEngine::Actor> actor)
{
   const auto itr = BIEngine::FindIf(m_peersToSend.Begin(), m_peersToSend.End(),
      [peerId](const PeerInfo& info) { return info.peerId == peerId; });
   if (itr != m_peersToSend.End()) {
      itr->playerActor = actor;
   }
}

void EventProtocolFollower::RegisterPeer(uint32_t peerId)
{
    m_peersToSend.PushBack(PeerInfo{ peerId });
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
        auto actor = pPeer.playerActor.Lock();
        if (!actor) {
           continue;
        }
        auto transform = actor->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).Lock();
        auto locomotion = actor->GetComponent<LocomotionInfoComponent>(LocomotionInfoComponent::g_CompId).Lock();
        auto binder = actor->GetComponent<PlayerCommandBinderComponent>(PlayerCommandBinderComponent::g_CompId).Lock();
        if (!transform || !locomotion || !binder) {
           continue;
        }

        CharacterMovementSnapshot snapshot;
        snapshot.lastProcessedSequence = binder->GetLastProcessedInputSequence();
        snapshot.state.position = transform->GetPosition();
        snapshot.state.rotation = transform->GetRotation();
        snapshot.state.velocity = locomotion->GetCurrentVel();
        snapshot.state.direction = locomotion->GetCurrentDir();
        snapshot.state.inputVelocity = locomotion->GetInputVel();
        snapshot.state.inputDirection = locomotion->GetInputDir();
        snapshot.state.orientation = locomotion->GetCurrentOrientation();
        snapshot.state.angularVelocity = locomotion->GetCurrentAngularVelocity();

        BIEngine::OutputMemoryBitStream packet;
        BIEngine::Serialize(packet, snapshot.lastProcessedSequence);
        BIEngine::Serialize(packet, snapshot.state.position);
        BIEngine::Serialize(packet, snapshot.state.rotation);
        BIEngine::Serialize(packet, snapshot.state.velocity);
        BIEngine::Serialize(packet, snapshot.state.direction.x);
        BIEngine::Serialize(packet, snapshot.state.direction.y);
        BIEngine::Serialize(packet, snapshot.state.inputVelocity);
        BIEngine::Serialize(packet, snapshot.state.inputDirection.x);
        BIEngine::Serialize(packet, snapshot.state.inputDirection.y);
        BIEngine::Serialize(packet, snapshot.state.orientation);
        BIEngine::Serialize(packet, snapshot.state.angularVelocity);

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
      uint32_t sequence = 0;
      BIEngine::Deserialize(inputStream, sequence);
      BIEngine::EventType eventType;
      BIEngine::Deserialize(inputStream, eventType);

      BIEngine::IEventDataPtr pEvent = BIEngine::g_eventFactory.Create(eventType);
      pEvent->Read(inputStream);

      if (eventType == EvtData_CharacterInput::sk_EventType) {
         auto actor = itr->playerActor.Lock();
         auto input = BIEngine::StaticPointerCast<EvtData_CharacterInput>(pEvent);
         if (!actor || actor->GetComponent<BIEngine::PlayerComponent>(BIEngine::PlayerComponent::g_CompId).Lock()->GetPlayerId() != input->GetPlayerId()) {
            --eventCount;
            continue;
         }
         input->SetSequence(sequence);
      }

      BIEngine::EventManager::Get()->QueueEvent(pEvent);

      --eventCount;
   }
}
