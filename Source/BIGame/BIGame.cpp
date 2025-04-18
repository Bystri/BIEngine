﻿#include "BIGame.h"

#include "../BIEngine/Graphics/WorldRenderPass.h"
#include "../BIEngine/Graphics/DirLightShadowGraphicsTechnique.h"
#include "../BIEngine/Graphics/PointLightShadowGraphicsTechnique.h"
#include "../BIEngine/Graphics/LightGraphicsTechnique.h"
#include "../BIEngine/Graphics/OpaqueGraphicsTechnique.h"
#include "../BIEngine/Graphics/SkyboxGraphicsTechnique.h"
#include "../BIEngine/Renderer/ShadersLoader.h"
#include "../BIEngine/Renderer/ImageLoader.h"
#include "../BIEngine/Navigation/NavMeshManager.h"

int main(int argc, char* argv[])
{
   std::shared_ptr<BIGameLogic> pBIGameLogic = std::make_shared<BIGameLogic>();
   BIGameApp BIGameApp(pBIGameLogic);

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

/**********BIGameApp**********/

BIGameApp::BIGameApp(std::shared_ptr<BIEngine::GameLogic> pGameLogic)
   : GameApp(pGameLogic)
{
}

BIGameApp::~BIGameApp()
{
}

bool BIGameApp::Init()
{
   if (!BIEngine::GameApp::Init()) {
      return false;
   }

   return true;
}

void BIGameApp::Close()
{
   BIEngine::GameApp::Close();
}

BIGameLogic::BIGameLogic()
{
}

bool BIGameLogic::Init()
{
   m_pPhysics2D.reset(BIEngine::CreateGamePhysics2D());
   m_pPhysics3D.reset(BIEngine::CreateGamePhysics3D());

   m_pNavWorld = std::make_unique<BIEngine::NavWorld>();

   if (!GameLogic::Init()) {
      return false;
   }

   m_pPhysics2D->Initialize();
   m_pPhysics3D->Initialize();

   m_pHumanView = std::make_shared<BIGameHumanView>(BIEngine::g_pApp->m_options.screenWidth, BIEngine::g_pApp->m_options.screenHeight);
   m_pHumanView->Init();
   AddGameView(m_pHumanView);

   m_pCameraManager = std::make_unique<BICameraManager>(m_pHumanView->GetScene()->GetCamera());

   BIRegisterEvents();

   BIEngine::EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BIGameLogic::NewPlayerActorDelegate), EvtData_PlayerActor_Created::sk_EventType);

   LoadLevel(BIEngine::g_pApp->m_options.mainWorldResNamePath);

   return true;
}

void BIGameLogic::Terminate()
{
   BIEngine::EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &BIGameLogic::NewPlayerActorDelegate), EvtData_PlayerActor_Created::sk_EventType);

   m_pCameraManager->Terminate();
}

bool BIGameLogic::LoadLevelDelegate(tinyxml2::XMLElement* pRoot)
{
   m_pNavWorld->Initialize();
   return true;
}

void BIGameLogic::OnUpdate(BIEngine::GameTimer& gt)
{
   GameLogic::OnUpdate(gt);
   m_pNavWorld->GetNavCrowd()->UpdateCrowdInfo(m_actors);
   m_pNavWorld->GetNavCrowd()->OnUpdate(gt);
}

void BIGameLogic::OnRender(const BIEngine::GameTimer& gt)
{
   BIEngine::GameLogic::OnRender(gt);
   m_pNavWorld->GetNavMeshManager()->RenderMesh();
}

static std::shared_ptr<BIEngine::Skybox> humanViewCreateSkybox()
{
   auto xmlExtraData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle("config/scene.xml")->GetExtra());

   if (!xmlExtraData) {
      return nullptr;
   }

   tinyxml2::XMLElement* pSkyboxSettingsNode = xmlExtraData->GetRootElement()->FirstChildElement("Skybox");
   BIEngine::Assert(pSkyboxSettingsNode, "Skybox settings did not loaded");

   if (!pSkyboxSettingsNode) {
      return nullptr;
   }

   const char* vertexShaderPath;
   const char* fragmentShaderPath;
   pSkyboxSettingsNode->QueryStringAttribute("vertexShaderPath", &vertexShaderPath);
   pSkyboxSettingsNode->QueryStringAttribute("fragmentShaderPath", &fragmentShaderPath);

   if (strlen(vertexShaderPath) == 0 || strlen(fragmentShaderPath) == 0) {
      return nullptr;
   }

   std::shared_ptr<BIEngine::ShaderData> pVertShaderData = std::static_pointer_cast<BIEngine::ShaderData>(BIEngine::ResCache::Get()->GetHandle(vertexShaderPath)->GetExtra());
   std::shared_ptr<BIEngine::ShaderData> pFragShaderxData = std::static_pointer_cast<BIEngine::ShaderData>(BIEngine::ResCache::Get()->GetHandle(fragmentShaderPath)->GetExtra());
   std::shared_ptr<BIEngine::ShaderProgram> pShaderProgram = std::make_shared<BIEngine::ShaderProgram>();
   pShaderProgram->Compile(pVertShaderData->GetShaderIndex(), pFragShaderxData->GetShaderIndex());


   std::vector<std::string> faces{
      "cubemapTextureRightPath",
      "cubemapTextureLeftPath",
      "cubemapTextureTopPath",
      "cubemapTextureBottomPath",
      "cubemapTextureFrontPath",
      "cubemapTextureBackPath"};

   std::array<unsigned char*, 6> cubemapTextureImages;
   int width = -1;
   int height = -1;

   for (int i = 0; i < faces.size(); ++i) {
      const char* cubemapTexturePath;
      pSkyboxSettingsNode->QueryStringAttribute(faces[i].c_str(), &cubemapTexturePath);

      if (strlen(cubemapTexturePath) == 0) {
         return nullptr;
      }

      auto cubemapTextureResExtraData = std::static_pointer_cast<BIEngine::ImageExtraData>(BIEngine::ResCache::Get()->GetHandle(cubemapTexturePath)->GetExtra());

      if (!cubemapTextureResExtraData) {
         return nullptr;
      }

      cubemapTextureImages[i] = cubemapTextureResExtraData->GetData();
      width = cubemapTextureResExtraData->GetWidth();
      height = cubemapTextureResExtraData->GetHeight();
   }

   std::shared_ptr<BIEngine::CubemapTexture> pTexture = BIEngine::CubemapTexture::Create(width, height, BIEngine::CubemapTexture::SizedFormat::RGB, BIEngine::CubemapTexture::Format::RGB, cubemapTextureImages);

   return std::make_shared<BIEngine::Skybox>(pTexture, pShaderProgram);
}

bool BIGameHumanView::Init()
{
   if (!BIEngine::HumanView::Init()) {
      return false;
   }

   constexpr std::size_t MAX_DIRECTIONAL_LIGHTS_NUM = 1;
   constexpr std::size_t MAX_POINT_LIGHTS_NUM = 1;
   constexpr std::size_t MAX_SPOT_LIGHTS_NUM = 2;

   std::shared_ptr<BIEngine::GraphicsRenderPass> pPreWorldRenderPass = std::make_shared<BIEngine::GraphicsRenderPass>();

   pPreWorldRenderPass->AddTechnique(std::make_shared<BIEngine::DirLightShadowGraphicsTechnique>(MAX_DIRECTIONAL_LIGHTS_NUM));
   pPreWorldRenderPass->AddTechnique(std::make_shared<BIEngine::PointLightShadowGraphicsTechnique>(MAX_POINT_LIGHTS_NUM));

   pPreWorldRenderPass->Init();

   m_pScene->AddRenderPass(pPreWorldRenderPass);

   constexpr int MsaaSamples = 4;
   std::shared_ptr<BIEngine::WorldRenderPass> pWorldRenderPass = std::make_shared<BIEngine::WorldRenderPass>(m_screenWidth, m_screenHeight, MsaaSamples);

   std::shared_ptr<BIEngine::LightGraphicsTechnique> pLightGraphicsTechnique = std::make_shared<BIEngine::LightGraphicsTechnique>(MAX_DIRECTIONAL_LIGHTS_NUM, MAX_POINT_LIGHTS_NUM, MAX_SPOT_LIGHTS_NUM);
   pWorldRenderPass->AddTechnique(pLightGraphicsTechnique);

   std::shared_ptr<BIEngine::OpaqueGraphicsTechnique> pOpaqueGraphicsTechnique = std::make_shared<BIEngine::OpaqueGraphicsTechnique>();
   pWorldRenderPass->AddTechnique(pOpaqueGraphicsTechnique);

   std::shared_ptr<BIEngine::SkyboxGraphicsTechnique> pSkyboxGraphicsTechnique = std::make_shared<BIEngine::SkyboxGraphicsTechnique>(humanViewCreateSkybox());
   pWorldRenderPass->AddTechnique(pSkyboxGraphicsTechnique);

   pWorldRenderPass->Init();

   m_pScene->AddRenderPass(pWorldRenderPass);

   return true;
}
