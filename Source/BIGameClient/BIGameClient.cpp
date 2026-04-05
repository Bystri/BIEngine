#include "BIGameClient.h"

#include "../BIEngine/Network/Replication/NetworkObjectCreationRegistry.h"
#include "../BIEngine/Graphics/WorldRenderPass.h"
#include "../BIEngine/Graphics/DirLightShadowGraphicsTechnique.h"
#include "../BIEngine/Graphics/PointLightShadowGraphicsTechnique.h"
#include "../BIEngine/Graphics/LightGraphicsTechnique.h"
#include "../BIEngine/Graphics/OpaqueGraphicsTechnique.h"
#include "../BIEngine/Graphics/SkyboxGraphicsTechnique.h"
#include "../BIEngine/Renderer/ShadersLoader.h"
#include "../BIEngine/Renderer/ImageLoader.h"
#include "../BIEngine/Navigation/NavMeshManager.h"
#include "../BIEngine/Actors/TransformComponent.h"
#include "../BIEngine/Actors/PlayerComponent.h"
#include "../BIGame/Network/ReplicationObjectAiDummyCharacter.h"
#include "../BIGame/Network/ReplicationObjectPlayerCharacter.h"
#include "../BIGame/Network/ReplicationObjectPlayer.h"
#include "../BIGame/Combat/CombatStateComponent.h"
#include "../BIGame/Combat/HealthStateComponent.h"
#include "AnimationControllerComponent.h"

int main(int argc, char* argv[])
{
   BIEngine::SharedPtr<BIGameClientLogic> pBIGameLogic = BIEngine::MakeShared<BIGameClientLogic>();
   BIGameClientApp BIGameApp(pBIGameLogic);

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

   return BIEngine::Run(argc, argv, 60);
}

/**********BIGameClientApp**********/

BIGameClientApp::BIGameClientApp(BIEngine::SharedPtr<BIEngine::GameLogic> pGameLogic)
   : GameApp(pGameLogic)
{
}

BIGameClientApp::~BIGameClientApp()
{
}

bool BIGameClientApp::Init()
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

void BIGameClientApp::Close()
{
   BIEngine::GameApp::Close();
   BIEngine::SocketUtil::Terminate();
}

BIGameClientLogic::BIGameClientLogic()
{
}

bool BIGameClientLogic::Init()
{
   m_pPhysics2D.Reset(BIEngine::CreateGamePhysics2D());
   m_pPhysics3D.Reset(BIEngine::CreateGamePhysics3D());

   m_pNavWorld = BIEngine::MakeUnique<BIEngine::NavWorld>();

   m_pActorFactory->AddComponentCreator(LocomotionInfoComponent::g_CompId, CreateLocomotionInfoComponent);
   m_pActorFactory->AddComponentCreator(CombatStateComponent::g_CompId, CreateCombatStateComponent);
   m_pActorFactory->AddComponentCreator(AnimationControllerComponent::g_CompId, CreateAnimationControllerComponent);
   m_pActorFactory->AddComponentCreator(HealthStateComponent::g_CompId, CreateHealthComponentComponent);

   if (!GameLogic::Init()) {
      return false;
   }

   PlayerManager::Create();

   BIEngine::SocketAddressPtr sockAddr = BIEngine::SocketUtil::CreateIPv4SocketFromString(BIEngine::g_pApp->m_options.hostAddress);
   {
      BIEngine::UniquePtr<BINetworkManagerClient> pNetworkManager = BIEngine::MakeUnique<BINetworkManagerClient>();
      pNetworkManager->Init(*sockAddr, BIEngine::g_pApp->m_options.playerName);
      m_pNetworkManager = std::move(pNetworkManager);
   }

   BIEngine::NetworkObjectCreationRegistry::Get().Register<ReplicationObjectPlayer>(ReplicationObjectPlayer::sk_ClassType);
   BIEngine::NetworkObjectCreationRegistry::Get().Register<ReplicationObjectPlayerCharacter>(ReplicationObjectPlayerCharacter::sk_ClassType);
   BIEngine::NetworkObjectCreationRegistry::Get().Register<ReplicationObjectAiDummyCharacter>(ReplicationObjectAiDummyCharacter::sk_ClassType);

   m_pPhysics2D->Initialize();
   m_pPhysics3D->Initialize();

   m_pHumanView = BIEngine::MakeShared<BIGameClientHumanView>(BIEngine::g_pApp->m_options.screenWidth, BIEngine::g_pApp->m_options.screenHeight);
   m_pHumanView->Init();
   AddGameView(m_pHumanView);

   m_pInputActionController = BIEngine::MakeUnique<BIInputActionController>();
   m_pCameraManager = BIEngine::MakeUnique<BICameraManager>(m_pHumanView->GetScene()->GetCamera());

#ifndef _RETAIL
   m_pDebugMenuController = BIEngine::MakeUnique<BIDebugMenuController>();
#endif

   BIRegisterEvents();

   m_newPlayerActorDelegateHandler = BIEngine::EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(BIGameClientLogic::NewPlayerActorDelegate), EvtData_PlayerActor_Created::sk_EventType);

   LoadLevel(BIEngine::g_pApp->m_options.mainWorldResNamePath);

   return true;
}

void BIGameClientLogic::Terminate()
{
   BIEngine::EventManager::Get()->RemoveListener(m_newPlayerActorDelegateHandler);

   m_pCameraManager->Terminate();
   m_pInputActionController->Term();
}

bool BIGameClientLogic::LoadLevelDelegate(tinyxml2::XMLElement* pRoot)
{
   m_pNavWorld->Initialize();
   return true;
}

void BIGameClientLogic::NewPlayerActorDelegate(BIEngine::IEventDataPtr pEventData)
{
   BIEngine::SharedPtr<EvtData_PlayerActor_Created> pCastEventData = BIEngine::StaticPointerCast<EvtData_PlayerActor_Created>(pEventData);

   BIEngine::Logger::WriteMsgLog("Got NewPlayerActorDelegate for player: %d", pCastEventData->GetPlayerId());
   if (pCastEventData->GetPlayerId() != m_pNetworkManager->GetPeerId()) {
      return;
   }

   BIEngine::SharedPtr<BIEngine::Actor> pActor = GetActor(pCastEventData->GetActorId());

   pActor->GetComponent<BIEngine::PlayerComponent>(BIEngine::PlayerComponent::g_CompId).Lock()->SetPlayerId(m_pNetworkManager->GetPeerId());

   m_pInputActionController->Init(m_pNetworkManager->GetPeerId(), m_pHumanView->GetScene()->GetCamera());
   m_pCameraManager->FollowActor(pActor);
}

void BIGameClientLogic::OnUpdate(BIEngine::GameTimer& gt)
{
   m_pInputActionController->OnUpdate();

   m_pNetworkManager->Update(gt);
   m_pNetworkManager->ProcessIncomingPackets();
   m_pNetworkManager->SendOutgoingPackets(gt);

   GameLogic::OnUpdate(gt);
   m_pNavWorld->GetNavCrowd()->UpdateCrowdInfo(m_actors);
   m_pNavWorld->GetNavCrowd()->OnUpdate(gt);
}

void BIGameClientLogic::OnRenderDebug(const BIEngine::GameTimer& gt)
{
#ifndef _RETAIL
   BIEngine::GameLogic::OnRenderDebug(gt);
   m_pDebugMenuController->OnUpdate();

   if (m_pDebugMenuController->IsShowNavMeshWindow()) {
      m_pNavWorld->GetNavMeshManager()->DrawRenderDiagnostics();
   }

   if (m_pDebugMenuController->IsShowPhysics3dWindow()) {
      m_pPhysics3D->DrawRenderDiagnostics();
   }

   if (m_pDebugMenuController->IsNetworkWindow()) {
      m_pNetworkManager->DrawDbgDiagnostics();
   }

   if (m_pDebugMenuController->IsNavCrowdWindow()) {
      m_pNavWorld->GetNavCrowd()->DrawDebug();
   }
#endif
}

/**********BIGameClientHumanView**********/

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

bool BIGameClientHumanView::Init()
{
   if (!BIEngine::HumanView::Init()) {
      return false;
   }

   constexpr std::size_t MAX_DIRECTIONAL_LIGHTS_NUM = 1;
   constexpr std::size_t MAX_POINT_LIGHTS_NUM = 1;
   constexpr std::size_t MAX_SPOT_LIGHTS_NUM = 2;

   BIEngine::SharedPtr<BIEngine::GraphicsRenderPass> pPreWorldRenderPass = BIEngine::MakeShared<BIEngine::GraphicsRenderPass>();

   pPreWorldRenderPass->AddTechnique(BIEngine::MakeShared<BIEngine::DirLightShadowGraphicsTechnique>(MAX_DIRECTIONAL_LIGHTS_NUM));
   pPreWorldRenderPass->AddTechnique(BIEngine::MakeShared<BIEngine::PointLightShadowGraphicsTechnique>(MAX_POINT_LIGHTS_NUM));

   pPreWorldRenderPass->Init();

   m_pScene->AddRenderPass(pPreWorldRenderPass);

   constexpr int MsaaSamples = 4;
   BIEngine::SharedPtr<BIEngine::WorldRenderPass> pWorldRenderPass = BIEngine::MakeShared<BIEngine::WorldRenderPass>(m_screenWidth, m_screenHeight, MsaaSamples);

   BIEngine::SharedPtr<BIEngine::LightGraphicsTechnique> pLightGraphicsTechnique = BIEngine::MakeShared<BIEngine::LightGraphicsTechnique>(MAX_DIRECTIONAL_LIGHTS_NUM, MAX_POINT_LIGHTS_NUM, MAX_SPOT_LIGHTS_NUM);
   pWorldRenderPass->AddTechnique(pLightGraphicsTechnique);

   BIEngine::SharedPtr<BIEngine::OpaqueGraphicsTechnique> pOpaqueGraphicsTechnique = BIEngine::MakeShared<BIEngine::OpaqueGraphicsTechnique>();
   pWorldRenderPass->AddTechnique(pOpaqueGraphicsTechnique);

   BIEngine::SharedPtr<BIEngine::SkyboxGraphicsTechnique> pSkyboxGraphicsTechnique = BIEngine::MakeShared<BIEngine::SkyboxGraphicsTechnique>(humanViewCreateSkybox());
   pWorldRenderPass->AddTechnique(pSkyboxGraphicsTechnique);

   pWorldRenderPass->Init();

   m_pScene->AddRenderPass(pWorldRenderPass);


   BIEngine::SharedPtr<BIGameController> pGameController = BIEngine::MakeShared<BIGameController>();
   SetController(pGameController);

   return true;
}
