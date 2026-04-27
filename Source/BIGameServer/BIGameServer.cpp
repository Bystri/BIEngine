#include "BIGameServer.h"

#include "../BIEngine/Navigation/NavMeshManager.h"
#include "../BIEngine/Actors/PlayerComponent.h"
#include "../BIEngine/Actors/TransformComponent.h"
#include "../BIEngine/Graphics/WorldRenderPass.h"
#include "../BIEngine/Graphics/SkyboxGraphicsTechnique.h"
#include "../BIEngine/Renderer/ShadersLoader.h"
#include "../BIEngine/Renderer/ImageLoader.h"
#include "../BIEngine/Network/Replication/ObjectReplicationProtocol.h"
#include "../BIGame/Network/ReplicationObjectPlayer.h"
#include "../BIGame/Network/ReplicationObjectPlayerCharacter.h"
#include "../BIGame/Network/ReplicationObjectAiDummyCharacter.h"
#include "../BIGame/Combat/CombatStateComponent.h"
#include "../BIGame/Combat/CombatControllerComponent.h"
#include "../BIGame/Combat/DamagableComponent.h"
#include "../BIGame/Network/BINetworkRPCs.h"

#include "BINetworkManagerServer.h"
#include "BIGSEventListener.h"
#include "PlayerCommandBinderComponent.h"
#include "CharacterMovementComponent.h"

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
         BIEngine::g_pApp->m_options.hostPort = BIEngine::Stoi(argv[i + 1]);
      }
   }

   return BIEngine::Run(argc, argv, 15);
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
   m_pActorFactory->AddComponentCreator(DamagableComponent::g_CompId, CreateDamagableComponentComponent);
   m_pActorFactory->AddComponentCreator(HealthStateComponent::g_CompId, CreateHealthComponentComponent);
   m_pActorFactory->AddComponentCreator(PlayerCommandBinderComponent::g_CompId, CreatePlayerCommandBinderComponent);
   m_pActorFactory->AddComponentCreator(CharacterMovementComponent::g_CompId, CreateCharacterMovementComponent);

   if (!GameLogic::Init()) {
      return false;
   }

   PlayerManager::Create();

   m_netPeerConnectedDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(BIServerGameLogic::OnNetPeerConnectedDelegate), EvtData_NetPeer_Connected::sk_EventType);
   m_netPeerDisonnectedDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(BIServerGameLogic::OnNetPeerDisonnectedDelegate), EvtData_NetPeer_Disonnected::sk_EventType);

   BIEngine::NetworkObjectCreationRegistry::Get().Register<ReplicationObjectPlayer>(ReplicationObjectPlayer::sk_ClassType);
   BIEngine::NetworkObjectCreationRegistry::Get().Register<ReplicationObjectPlayerCharacter>(ReplicationObjectPlayerCharacter::sk_ClassType);
   BIEngine::NetworkObjectCreationRegistry::Get().Register<ReplicationObjectAiDummyCharacter>(ReplicationObjectAiDummyCharacter::sk_ClassType);

   {
      constexpr int MAX_CLIENTS = 20;
      BIEngine::UniquePtr<BINetworkManagerServer> pNetworkManager = BIEngine::MakeUnique<BINetworkManagerServer>();
      if (!pNetworkManager->Init(BIEngine::g_pApp->m_options.hostPort, MAX_CLIENTS)) {
         return false;
      }
      m_pNetworkManager = std::move(pNetworkManager);
   }

   m_pPhysics2D->Initialize();
   m_pPhysics3D->Initialize();

#ifndef _RETAIL
   m_pDebugMenuController = BIEngine::MakeUnique<BIDebugMenuController>();

   m_pDbgHumanView = BIEngine::MakeShared<BIServerDbgHumanView>(BIEngine::g_pApp->m_options.screenWidth, BIEngine::g_pApp->m_options.screenHeight);
   m_pDbgHumanView->Init();
   AddGameView(m_pDbgHumanView);
#endif

   BIRegisterServerEvents();

   LoadLevel(BIEngine::g_pApp->m_options.mainWorldResNamePath);

   BIEngine::ObjectReplicationCreate(ReplicationObjectAiDummyCharacter::sk_ClassType);

   return true;
}

void BIServerGameLogic::Terminate()
{
   BIEngine::EventManager::Get()->RemoveListener(m_netPeerConnectedDelegateHandler);
   BIEngine::EventManager::Get()->RemoveListener(m_netPeerDisonnectedDelegateHandler);
}

bool BIServerGameLogic::LoadLevelDelegate(tinyxml2::XMLElement* pRoot)
{
   m_pNavWorld->Initialize();
   return true;
}

void BIServerGameLogic::OnUpdate(BIEngine::GameTimer& gt)
{
   m_pNetworkManager->Update(gt);
   m_pNetworkManager->ProcessIncomingPackets();

   GameLogic::OnUpdate(gt);
   m_pNavWorld->GetNavCrowd()->UpdateCrowdInfo(m_actors);
   m_pNavWorld->GetNavCrowd()->OnUpdate(gt);

   BIEngine::ObjectReplicationProtocolWriter::Get()->OnUpdate();
   m_pNetworkManager->SendOutgoingPackets(gt);
}

void BIServerGameLogic::OnRenderDebug(const BIEngine::GameTimer& gt)
{
#ifndef _RETAIL
   BIEngine::GameLogic::OnRenderDebug(gt);
   m_pDebugMenuController->OnUpdate();

   if (m_pDebugMenuController->IsShowNavMeshWindow()) {
      m_pNavWorld->GetNavMeshManager()->DrawRenderDiagnostics();
   }

   if (m_pDebugMenuController->IsNavCrowdWindow()) {
      m_pNavWorld->GetNavCrowd()->DrawDebug();
   }

   if (m_pDebugMenuController->IsShowPhysics3dWindow()) {
      m_pPhysics3D->DrawRenderDiagnostics();
   }

   if (m_pDebugMenuController->IsNetworkWindow()) {
      m_pNetworkManager->DrawDbgDiagnostics();
   }

   if (m_pDebugMenuController->IsReplicationWindow()) {
      BIEngine::ObjectReplicationProtocolWriter::Get()->DrawDbgDiagnostics();
   }
#endif
}

void BIServerGameLogic::OnNetPeerConnectedDelegate(BIEngine::IEventDataPtr pEventData)
{
   BIEngine::SharedPtr<EvtData_NetPeer_Connected> pCastEventData = BIEngine::StaticPointerCast<EvtData_NetPeer_Connected>(pEventData);

   const BIEngine::PeerId connectedPeerId = pCastEventData->GetPeerId();

   BIEngine::SharedPtr<ReplicationObjectPlayer> pReplicatedPlayer = BIEngine::StaticPointerCast<ReplicationObjectPlayer>(BIEngine::ObjectReplicationCreate(ReplicationObjectPlayer::sk_ClassType));
   m_peerIdToPlayerMap.Insert(connectedPeerId, pReplicatedPlayer);
   RpcWriteSetPlayer(connectedPeerId, pReplicatedPlayer->GetReplicatedObject()->GetId());

   BIEngine::SharedPtr<BIEngine::ReplicationObjectActor> pPlayerActor = BIEngine::StaticPointerCast<BIEngine::ReplicationObjectActor>(BIEngine::ObjectReplicationCreate(ReplicationObjectPlayerCharacter::sk_ClassType));
   pReplicatedPlayer->GetReplicatedObject()->SetPlayableActor(pPlayerActor->GetReplicatedObject());

   constexpr float softReplicationRelevancyRadius = 20.0f;
   constexpr float hardReplicationRelevancyRadius = 30.0f;
   BIEngine::ObjectReplicationProtocolWriter::Get()->AddObjectReplicationPOI(connectedPeerId, pPlayerActor->GetReplicatedObject(), softReplicationRelevancyRadius, hardReplicationRelevancyRadius);
}

void BIServerGameLogic::OnNetPeerDisonnectedDelegate(BIEngine::IEventDataPtr pEventData)
{
   BIEngine::SharedPtr<EvtData_NetPeer_Disonnected> pCastEventData = BIEngine::StaticPointerCast<EvtData_NetPeer_Disonnected>(pEventData);
   
   const BIEngine::PeerId disconnectedPeerId = pCastEventData->GetPeerId();
   BIEngine::ObjectReplicationProtocolWriter::Get()->RemoveObjectReplicationPOI(disconnectedPeerId);

   DestroyActor(m_peerIdToPlayerMap[disconnectedPeerId]->GetReplicatedObject()->GetPlayableActor()->GetId());

   BIEngine::ObjectReplicationDestroy(m_peerIdToPlayerMap[disconnectedPeerId]);

   m_peerIdToPlayerMap.Erase(disconnectedPeerId);
}

/**********BIServerDbgHumanView**********/

static BIEngine::SharedPtr<BIEngine::Skybox> humanViewCreateSkybox()
{
   auto xmlExtraData = BIEngine::StaticPointerCast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle("config/scene.xml")->GetExtra());

   if (!xmlExtraData) {
      return nullptr;
   }

   tinyxml2::XMLElement* pSkyboxSettingsNode = xmlExtraData->GetRootElement()->FirstChildElement("Skybox");
   BIEngine::Assert(pSkyboxSettingsNode, "Skybox settings did not loaded");

   if (!pSkyboxSettingsNode) {
      return nullptr;
   }

   const char* vertexShaderPath = nullptr;
   const char* fragmentShaderPath = nullptr;
   pSkyboxSettingsNode->QueryStringAttribute("vertexShaderPath", &vertexShaderPath);
   pSkyboxSettingsNode->QueryStringAttribute("fragmentShaderPath", &fragmentShaderPath);

   if (strlen(vertexShaderPath) == 0 || strlen(fragmentShaderPath) == 0) {
      return nullptr;
   }

   BIEngine::SharedPtr<BIEngine::ShaderData> pVertShaderData = BIEngine::StaticPointerCast<BIEngine::ShaderData>(BIEngine::ResCache::Get()->GetHandle(vertexShaderPath)->GetExtra());
   BIEngine::SharedPtr<BIEngine::ShaderData> pFragShaderxData = BIEngine::StaticPointerCast<BIEngine::ShaderData>(BIEngine::ResCache::Get()->GetHandle(fragmentShaderPath)->GetExtra());
   BIEngine::SharedPtr<BIEngine::ShaderProgram> pShaderProgram = BIEngine::MakeShared<BIEngine::ShaderProgram>();
   pShaderProgram->Compile(pVertShaderData->GetShaderIndex(), pFragShaderxData->GetShaderIndex());


   BIEngine::DynamicArray<BIEngine::String> faces{
      "cubemapTextureRightPath",
      "cubemapTextureLeftPath",
      "cubemapTextureTopPath",
      "cubemapTextureBottomPath",
      "cubemapTextureFrontPath",
      "cubemapTextureBackPath"};

   BIEngine::Array<unsigned char*, 6> cubemapTextureImages;
   int width = -1;
   int height = -1;

   for (int i = 0; i < faces.Size(); ++i) {
      const char* cubemapTexturePath = nullptr;
      pSkyboxSettingsNode->QueryStringAttribute(faces[i].CStr(), &cubemapTexturePath);

      if (strlen(cubemapTexturePath) == 0) {
         return nullptr;
      }

      auto cubemapTextureResExtraData = BIEngine::StaticPointerCast<BIEngine::ImageExtraData>(BIEngine::ResCache::Get()->GetHandle(cubemapTexturePath)->GetExtra());

      if (!cubemapTextureResExtraData) {
         return nullptr;
      }

      cubemapTextureImages[i] = cubemapTextureResExtraData->GetData();
      width = cubemapTextureResExtraData->GetWidth();
      height = cubemapTextureResExtraData->GetHeight();
   }

   BIEngine::SharedPtr<BIEngine::CubemapTexture> pTexture = BIEngine::CubemapTexture::Create(width, height, BIEngine::CubemapTexture::SizedFormat::RGB, BIEngine::CubemapTexture::Format::RGB, cubemapTextureImages);

   return BIEngine::MakeShared<BIEngine::Skybox>(pTexture, pShaderProgram);
}

bool BIServerDbgHumanView::Init()
{
   if (!BIEngine::HumanView::Init()) {
      return false;
   }

   constexpr int MsaaSamples = 4;
   BIEngine::SharedPtr<BIEngine::WorldRenderPass> pWorldRenderPass = BIEngine::MakeShared<BIEngine::WorldRenderPass>(m_screenWidth, m_screenHeight, MsaaSamples);

   BIEngine::SharedPtr<BIEngine::SkyboxGraphicsTechnique> pSkyboxGraphicsTechnique = BIEngine::MakeShared<BIEngine::SkyboxGraphicsTechnique>(humanViewCreateSkybox());
   pWorldRenderPass->AddTechnique(pSkyboxGraphicsTechnique);

   pWorldRenderPass->Init();

   m_pScene->AddRenderPass(pWorldRenderPass);

   BIEngine::SharedPtr<BIGameController> pGameController = BIEngine::MakeShared<BIGameController>();
   SetController(pGameController);

   m_pScene->GetCamera()->MoveTo(glm::vec3(3.0f, 6.0f, 3.0f));

   m_pFlyCameraSystem = BIEngine::MakeUnique<BIFlyCameraSystem>(m_pScene->GetCamera(), pGameController);
}

void BIServerDbgHumanView::OnUpdate(const BIEngine::GameTimer& gt)
{
   BIEngine::HumanView::OnUpdate(gt);

   m_pFlyCameraSystem->OnUpdate(gt);
}
