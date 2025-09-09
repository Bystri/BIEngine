#include "BIGameServer.h"

#include "../BIEngine/Navigation/NavMeshManager.h"
#include "../BIEngine/Actors/PlayerComponent.h"
#include "../BIEngine/Actors/TransformComponent.h"
#include "../BIEngine/Network/Replication/ObjectReplicationManagerMaster.h"
#include "../BIGame/Network/ReplicationObjectPlayer.h"
#include "../BIGame/Network/ReplicationObjectPlayerCharacter.h"
#include "../BIGame/Combat/CombatStateComponent.h"
#include "../BIGame/Combat/CombatControllerComponent.h"

#include "BINetworkManagerServer.h"
#include "BIGSEventListener.h"

int main(int argc, char* argv[])
{
   BIEngine::SharedPtr<BIServerGameLogic> pBIGameLogic = BIEngine::MakeShared<BIServerGameLogic>();
   BIGameServerApp BIGameApp(pBIGameLogic);

   if (!BIEngine::g_pApp) {
      return -1;
   }

   BIEngine::g_pApp->m_options.useDevelopmentAssets = true;
   for (int i = 0; i < argc; ++i) {
      if (std::memcmp(argv[i], "-pn", 3) == 0) {
         BIEngine::g_pApp->m_options.playerName = argv[i + 1];
      }
      if (std::memcmp(argv[i], "-ha", 3) == 0) {
         BIEngine::g_pApp->m_options.hostAddress = argv[i + 1];
      }
      if (std::memcmp(argv[i], "-hp", 3) == 0) {
         BIEngine::g_pApp->m_options.hostPort = std::atoi(argv[i + 1]);
      }
   }

   return BIEngine::Run(argc, argv);
}

/**********BIGameServerApp**********/

BIGameServerApp::BIGameServerApp(BIEngine::SharedPtr<BIEngine::GameLogic> pGameLogic)
   : GameApp(pGameLogic)
{
}

BIGameServerApp::~BIGameServerApp()
{
}

bool BIGameServerApp::Init()
{
   if (BIEngine::SocketUtil::Init()) {
      BIEngine::Logger::WriteErrorLog("Error while init");
      return false;
   }

   if (!BIEngine::GameApp::Init()) {
      return false;
   }

   return true;
}

void BIGameServerApp::Close()
{
   BIEngine::GameApp::Close();
   BIEngine::SocketUtil::Terminate();
}

BIServerGameLogic::BIServerGameLogic()
{
}

bool BIServerGameLogic::Init()
{
   m_pPhysics2D.Reset(BIEngine::CreateGamePhysics2D());
   m_pPhysics3D.Reset(BIEngine::CreateGamePhysics3D());

   m_pNavWorld = BIEngine::MakeUnique<BIEngine::NavWorld>();

   m_pActorFactory->AddComponentCreator(LocomotionInfoComponent::g_CompId, CreateLocomotionInfoComponent);
   m_pActorFactory->AddComponentCreator(CombatStateComponent::g_CompId, CreateCombatStateComponent);
   m_pActorFactory->AddComponentCreator(CombatControllerComponent::g_CompId, CreateCombatControllerComponent);

   if (!GameLogic::Init()) {
      return false;
   }

   PlayerManager::Create();

   m_playerCreatedDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(BIServerGameLogic::PlayerCreatedDelegate), EvtData_Player_Created::sk_EventType);

   BIEngine::NetworkObjectCreationRegistry::Get().Register<ReplicationObjectPlayer>(ReplicationObjectPlayer::sk_ClassType);
   BIEngine::NetworkObjectCreationRegistry::Get().Register<ReplicationObjectPlayerCharacter>(ReplicationObjectPlayerCharacter::sk_ClassType);

   m_pNetworkManager = BIEngine::MakeUnique<BINetworkManagerServer>();
   if (!m_pNetworkManager->Init(BIEngine::g_pApp->m_options.hostPort)) {
      return false;
   }

   m_pPhysics2D->Initialize();
   m_pPhysics3D->Initialize();

   // m_pHumanView = std::make_shared<BIGameHumanView>(BIEngine::g_pApp->m_options.screenWidth, BIEngine::g_pApp->m_options.screenHeight);
   // m_pHumanView->Init();
   // AddGameView(m_pHumanView);

   BIRegisterEvents();

   LoadLevel(BIEngine::g_pApp->m_options.mainWorldResNamePath);

   return true;
}

void BIServerGameLogic::Terminate()
{
   BIEngine::EventManager::Get()->RemoveListener(m_playerCreatedDelegateHandler);
}

bool BIServerGameLogic::LoadLevelDelegate(tinyxml2::XMLElement* pRoot)
{
   m_pNavWorld->Initialize();
   return true;
}

void BIServerGameLogic::OnUpdate(BIEngine::GameTimer& gt)
{
   m_pNetworkManager->ProcessIncomingPackets();

   GameLogic::OnUpdate(gt);
   m_pNavWorld->GetNavCrowd()->UpdateCrowdInfo(m_actors);
   m_pNavWorld->GetNavCrowd()->OnUpdate(gt);

   BIEngine::ObjectReplicationManagerMaster::Get()->OnUpdate();
   m_pNetworkManager->SendOutgoingPackets();
}

void BIServerGameLogic::PlayerCreatedDelegate(BIEngine::IEventDataPtr pEventData)
{
   BIEngine::SharedPtr<EvtData_Player_Created> pCastEventData = BIEngine::StaticPointerCast<EvtData_Player_Created>(pEventData);

   BIEngine::SharedPtr<BIEngine::ReplicationObjectActor> pGameObject = BIEngine::StaticPointerCast<BIEngine::ReplicationObjectActor>(BIEngine::ObjectReplicationCreate(ReplicationObjectPlayerCharacter::sk_ClassType));
   pCastEventData->GetPlayer()->SetPlayableActor(pGameObject->GetReplicatedObject());
}
