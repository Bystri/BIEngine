#include "BIGame.h"

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
   BIEngine::SharedPtr<BIGameLogic> pBIGameLogic = BIEngine::MakeShared<BIGameLogic>();
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
         BIEngine::g_pApp->m_options.hostPort = BIEngine::Stoi(argv[i + 1]);
      }
   }

   return BIEngine::Run(argc, argv, 60);
}

/**********BIGameApp**********/

BIGameApp::BIGameApp(BIEngine::SharedPtr<BIEngine::GameLogic> pGameLogic)
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
   m_pPhysics2D.Reset(BIEngine::CreateGamePhysics2D());
   m_pPhysics3D.Reset(BIEngine::CreateGamePhysics3D());

   m_pNavWorld = BIEngine::MakeUnique<BIEngine::NavWorld>();

   if (!GameLogic::Init()) {
      return false;
   }

   m_pPhysics2D->Initialize();
   m_pPhysics3D->Initialize();

   m_pHumanView = BIEngine::MakeShared<BIGameHumanView>(BIEngine::g_pApp->m_options.screenWidth, BIEngine::g_pApp->m_options.screenHeight);
   m_pHumanView->Init();
   AddGameView(m_pHumanView);

   m_pCameraManager = BIEngine::MakeUnique<BICameraManager>(m_pHumanView->GetScene()->GetCamera());

   BIRegisterEvents();

   LoadLevel(BIEngine::g_pApp->m_options.mainWorldResNamePath);

   return true;
}

void BIGameLogic::Terminate()
{
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

   const char* vertexShaderPath;
   const char* fragmentShaderPath;
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
      const char* cubemapTexturePath;
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

bool BIGameHumanView::Init()
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

   return true;
}
