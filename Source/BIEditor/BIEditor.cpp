#include "BIEditor.h"

#include "../BIEngine/Graphics/WorldRenderPass.h"
#include "../BIEngine/Graphics/ShadowGraphicsTechnique.h"
#include "../BIEngine/Graphics/LightGraphicsTechnique.h"
#include "../BIEngine/Graphics/OpaqueGraphicsTechnique.h"
#include "../BIEngine/Graphics/SkyboxGraphicsTechnique.h"
#include "../BIEngine/Renderer/ShadersLoader.h"
#include "../BIEngine/Renderer/PostProcessor.h"
#include "../BIEngine/Renderer/Renderbuffer.h"
#include "../BIEngine/Renderer/ImageLoader.h"

#include "Graphics/ActorPickingTechnique.h"
#include "Widgets/ActorEditorWidget.h"

int main(int argc, char* argv[])
{
   std::shared_ptr<BIEditorLogic> pBIGameLogic = std::make_shared<BIEditorLogic>();
   BIEditorApp BIGameApp(pBIGameLogic);

   if (!BIEngine::g_pApp) {
      return -1;
   }

   BIEngine::g_pApp->m_options.useDevelopmentAssets = true;
   BIEngine::g_pApp->m_options.maximazeWindow = true;
   return BIEngine::Run(argc, argv);
}

/**********BIGameApp**********/

BIEditorApp::BIEditorApp(std::shared_ptr<BIEngine::GameLogic> pGameLogic)
   : GameApp(pGameLogic)
{
}

BIEditorApp::~BIEditorApp()
{
}

bool BIEditorApp::Init()
{
   if (!BIEngine::GameApp::Init()) {
      return false;
   }

   return true;
}

void BIEditorApp::Close()
{
   BIEngine::GameApp::Close();
}

BIEditorLogic::BIEditorLogic()
{
   m_pPhysics2D.reset(BIEngine::CreateNullPhysics2D());
   m_pPhysics3D.reset(BIEngine::CreateNullPhysics3D());
}

bool BIEditorLogic::Init()
{
   if (!GameLogic::Init()) {
      return false;
   }

   m_pPhysics2D->Initialize();
   m_pPhysics3D->Initialize();

   std::shared_ptr<BIEditorHumanView> humanView = std::make_shared<BIEditorHumanView>(BIEngine::g_pApp->m_options.screenWidth, BIEngine::g_pApp->m_options.screenHeight);
   AddGameView(humanView);

   // Загружаем стартовый мир
   LoadLevel(BIEngine::g_pApp->m_options.mainWorldResNamePath);

   return true;
}

void BIEditorLogic::OnUpdate(BIEngine::GameTimer& gt)
{
   GameLogic::OnUpdate(gt);
}

BIEditorHumanView::BIEditorHumanView(unsigned int screenWidth, unsigned int screenHeight)
   : BIEngine::HumanView(screenWidth, screenHeight),
     m_pActorEditorWidget(nullptr),
     m_bIsWindowFocused(false)
{
}

static std::shared_ptr<BIEngine::Skybox> humanViewCreateSkybox()
{
   auto xmlExtraData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle("config/scene.xml")->GetExtra());

   if (!xmlExtraData) {
      return nullptr;
   }

   tinyxml2::XMLElement* pSkyboxSettingsNode = xmlExtraData->GetRootElement()->FirstChildElement("Skybox");
   assert(pSkyboxSettingsNode);

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

   std::shared_ptr<BIEngine::CubemapTexture> pTexture = BIEngine::CubemapTexture::Create(width, height, BIEngine::CubemapTexture::Format::RGB, cubemapTextureImages);

   return std::make_shared<BIEngine::Skybox>(pTexture, pShaderProgram);
}

bool BIEditorHumanView::Init()
{
   if (!BIEngine::HumanView::Init()) {
      return false;
   }

   constexpr std::size_t MAX_DIRECTIONAL_LIGHTS_NUM = 1;
   constexpr std::size_t MAX_POINT_LIGHTS_NUM = 1;
   constexpr std::size_t MAX_SPOT_LIGHTS_NUM = 1;

   std::shared_ptr<BIEngine::GraphicsRenderPass> pPreWorldRenderPass = std::make_shared<BIEngine::GraphicsRenderPass>();

   std::shared_ptr<ActorPickingTechnique> pActorPickingTechnique = std::make_shared<ActorPickingTechnique>(m_screenWidth, m_screenHeight);
   pPreWorldRenderPass->AddTechnique(pActorPickingTechnique);

   std::shared_ptr<BIEngine::ShadowGraphicsTechnique> pShadowGraphicsTechnique = std::make_shared<BIEngine::ShadowGraphicsTechnique>(MAX_DIRECTIONAL_LIGHTS_NUM, MAX_POINT_LIGHTS_NUM);
   pPreWorldRenderPass->AddTechnique(pShadowGraphicsTechnique);

   pPreWorldRenderPass->Init();

   m_pActorPickerInfoStorage = pActorPickingTechnique->GetPickingInfoStorage();

   m_pScene->AddRenderPass(pPreWorldRenderPass);

   m_pGameRenderTarget = std::make_shared<BIEngine::Framebuffer>();
   m_pGameRenderTargetColorBuffer = BIEngine::Texture2D::Create(m_screenWidth, m_screenHeight, BIEngine::Texture::SizedFormat::RGB, BIEngine::Texture::Format::RGB, nullptr);
   m_pGameRenderTargetDepthBuffer = BIEngine::Renderbuffer::Create(m_screenWidth, m_screenHeight, BIEngine::Renderbuffer::Format::DEPTH24);
   FramebufferAttach(m_pGameRenderTarget, BIEngine::FramebufferAttachementType::COLOR, m_pGameRenderTargetColorBuffer);
   FramebufferAttach(m_pGameRenderTarget, BIEngine::FramebufferAttachementType::DEPTH, m_pGameRenderTargetDepthBuffer);
   if (!m_pGameRenderTarget->Check()) {
      return false;
   }

   constexpr int MsaaSamples = 4;
   std::shared_ptr<BIEngine::WorldRenderPass> pWorldRenderPass = std::make_shared<BIEngine::WorldRenderPass>(m_screenWidth, m_screenHeight, MsaaSamples);
   pWorldRenderPass->SetRenderTarget(m_pGameRenderTarget);

   std::shared_ptr<BIEngine::LightGraphicsTechnique> pLightGraphicsTechnique = std::make_shared<BIEngine::LightGraphicsTechnique>(MAX_DIRECTIONAL_LIGHTS_NUM, MAX_POINT_LIGHTS_NUM, MAX_SPOT_LIGHTS_NUM);
   pWorldRenderPass->AddTechnique(pLightGraphicsTechnique);

   std::shared_ptr<BIEngine::OpaqueGraphicsTechnique> pOpaqueGraphicsTechnique = std::make_shared<BIEngine::OpaqueGraphicsTechnique>();
   pWorldRenderPass->AddTechnique(pOpaqueGraphicsTechnique);

   std::shared_ptr<BIEngine::SkyboxGraphicsTechnique> pSkyboxGraphicsTechnique = std::make_shared<BIEngine::SkyboxGraphicsTechnique>(humanViewCreateSkybox());
   pWorldRenderPass->AddTechnique(pSkyboxGraphicsTechnique);

   pWorldRenderPass->Init();

   m_pScene->AddRenderPass(pWorldRenderPass);

   std::shared_ptr<BIEditorController> pGameController = std::make_shared<BIEditorController>();
   SetController(pGameController);

   m_pFlyCameraSystem = new BIFlyCameraSystem(m_pScene->GetCamera(), pGameController);

   // Edtiro GUI
   m_pActorEditorWidget = new ActorEditorWidget();
}

void BIEditorHumanView::Shutdown()
{
   if (m_pActorEditorWidget) {
      delete m_pActorEditorWidget;
      m_pActorEditorWidget = nullptr;
   }

   if (m_pFlyCameraSystem) {
      delete m_pFlyCameraSystem;
      m_pFlyCameraSystem = nullptr;
   }

   BIEngine::HumanView::Shutdown();
}

void BIEditorHumanView::OnUpdate(const BIEngine::GameTimer& gt)
{
   BIEngine::HumanView::OnUpdate(gt);

   if (m_pFlyCameraSystem && m_bIsWindowFocused) {
      m_pFlyCameraSystem->OnUpdate(gt);
   }
}

void BIEditorHumanView::OnRender(const BIEngine::GameTimer& gt)
{
   showSceneTree();

   BIEngine::HumanView::OnRender(gt);


   ImGui::SetNextWindowContentSize(ImVec2(m_screenWidth, m_screenHeight));
   ImGui::Begin("Scene");
   {
      ImGui::BeginChild("GameRender");

      ImGui::Image(
         (ImTextureID)m_pGameRenderTargetColorBuffer->GetId(),
         ImGui::GetContentRegionAvail(),
         ImVec2(0, 1),
         ImVec2(1, 0));

      m_bIsWindowFocused = ImGui::IsWindowFocused();

      if (m_bIsWindowFocused) {
         ImVec2 screenPos = ImGui::GetCursorScreenPos();
         ImVec2 mousePos = ImGui::GetMousePos();

         mousePos.x -= screenPos.x;
         mousePos.y = screenPos.y - mousePos.y;

         BIEngine::ActorId selectedId = BIEngine::Actor::INVALID_ACTOR_ID;
         if (std::shared_ptr<ActorPickerInfoStorage> spt = m_pActorPickerInfoStorage.lock()) {
            selectedId = spt->GetActorId(mousePos.x, mousePos.y);
         }

         if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && selectedId != BIEngine::Actor::INVALID_ACTOR_ID) {
            m_pActorEditorWidget->SetCurrentEditableActorId(selectedId);
         }
      }

      ImGui::EndChild();
   }
   ImGui::End();

   BIEngine::GetDefaultFramebuffer()->Bind();
   static const BIEngine::ColorRgba CLEAR_COLOR = BIEngine::ColorRgba(0.0f, 0.5f, 0.5f, 1.0f);
   m_pRenderer->Clear(BIEngine::RenderDevice::ClearFlag::COLOR | BIEngine::RenderDevice::ClearFlag::DEPTH, CLEAR_COLOR);
}

void BIEditorHumanView::showSceneTree()
{
   if (ImGui::Begin("World")) {
      int numActors = BIEngine::g_pApp->m_pGameLogic->GetNumActors();
      for (int i = 0; i < numActors; ++i) {
         std::shared_ptr<BIEngine::Actor> pActor = BIEngine::g_pApp->m_pGameLogic->GetActor(i);

         ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
         const bool isSelected = pActor->GetId() == m_pActorEditorWidget->GetCurrentSelectedActorId();
         if (isSelected) {
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
         }

         if (ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, pActor->GetName().c_str())) {
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
               m_pActorEditorWidget->SetCurrentEditableActorId(pActor->GetId());
            }
         }

         if (pActor->GetId() == m_pActorEditorWidget->GetCurrentSelectedActorId()) {
            m_pActorEditorWidget->Show();
         }
      }

      ImGui::End();
   }
}
