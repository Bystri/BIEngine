#include "BIEditor.h"

#include "../BIEngine/Graphics/WorldRenderPass.h"
#include "../BIEngine/Graphics/DirLightShadowGraphicsTechnique.h"
#include "../BIEngine/Graphics/PointLightShadowGraphicsTechnique.h"
#include "../BIEngine/Graphics/LightGraphicsTechnique.h"
#include "../BIEngine/Graphics/OpaqueGraphicsTechnique.h"
#include "../BIEngine/Graphics/SkyboxGraphicsTechnique.h"
#include "../BIEngine/Renderer/ShadersLoader.h"
#include "../BIEngine/Renderer/PostProcessor.h"
#include "../BIEngine/Renderer/Renderbuffer.h"
#include "../BIEngine/Renderer/ImageLoader.h"
#include "../BIEngine/EngineCore/Assert.h"

#include "Graphics/ActorPickingTechnique.h"
#include "Graphics/SelectedActorOutliner.h"
#include "Widgets/ActorEditorWidget.h"

int main(int argc, char* argv[])
{
   std::shared_ptr<BIEditorLogic> pBIGameLogic = std::make_shared<BIEditorLogic>();
   BIEditorApp BIGameApp(pBIGameLogic);

   if (!BIEngine::g_pApp) {
      return -1;
   }

   BIEngine::g_pApp->m_options.useDevelopmentAssets = true;
   BIEngine::g_pApp->m_options.isEditorMode = true;
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

   // ��������� ��������� ���
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
     m_wasCtrlButtonProcessed(false),
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

bool BIEditorHumanView::Init()
{
   if (!BIEngine::HumanView::Init()) {
      return false;
   }

   // Editor GUI
   m_pActorEditorWidget = new ActorEditorWidget();


   // Render pipline
   constexpr std::size_t MAX_DIRECTIONAL_LIGHTS_NUM = 1;
   constexpr std::size_t MAX_POINT_LIGHTS_NUM = 1;
   constexpr std::size_t MAX_SPOT_LIGHTS_NUM = 2;

   std::shared_ptr<BIEngine::GraphicsRenderPass> pPreWorldRenderPass = std::make_shared<BIEngine::GraphicsRenderPass>();

   std::shared_ptr<ActorPickingTechnique> pActorPickingTechnique = std::make_shared<ActorPickingTechnique>(m_screenWidth, m_screenHeight);
   pPreWorldRenderPass->AddTechnique(pActorPickingTechnique);

   pPreWorldRenderPass->AddTechnique(std::make_shared<BIEngine::DirLightShadowGraphicsTechnique>(MAX_DIRECTIONAL_LIGHTS_NUM));
   pPreWorldRenderPass->AddTechnique(std::make_shared<BIEngine::PointLightShadowGraphicsTechnique>(MAX_POINT_LIGHTS_NUM));

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

   std::shared_ptr<SelectedActorOutliner> pSelectedActorTechnique = std::make_shared<SelectedActorOutliner>(m_pActorEditorWidget);
   pWorldRenderPass->AddTechnique(pSelectedActorTechnique);

   pWorldRenderPass->Init();

   m_pScene->AddRenderPass(pWorldRenderPass);

   std::shared_ptr<BIEditorController> pGameController = std::make_shared<BIEditorController>();
   SetController(pGameController);

   m_pFlyCameraSystem = new BIFlyCameraSystem(m_pScene->GetCamera(), pGameController);
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

   if ((m_pKeyboardHandler->IsKeyPressed(GLFW_KEY_LEFT_CONTROL) || m_pKeyboardHandler->IsKeyPressed(GLFW_KEY_RIGHT_CONTROL)) == false) {
      if (m_pFlyCameraSystem && m_bIsWindowFocused) {
         m_pFlyCameraSystem->OnUpdate(gt);
      }

      m_wasCtrlButtonProcessed = false;

      return;
   }

   if (!m_wasCtrlButtonProcessed) {
      if (m_pKeyboardHandler->IsKeyPressed(GLFW_KEY_D)) {
         duplicateActor();
         m_wasCtrlButtonProcessed = true;
      } else if (m_pKeyboardHandler->IsKeyPressed(GLFW_KEY_S)) {
         saveWorld();
         m_wasCtrlButtonProcessed = true;
      }
   }
}

void BIEditorHumanView::OnPostRender(const BIEngine::GameTimer& gt)
{
   showMenu();
   showSceneTree();

   BIEngine::HumanView::OnPostRender(gt);


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

void BIEditorHumanView::showMenu()
{
   if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("File")) {
         if (ImGui::MenuItem("Save World", "CTRL+S")) {
            saveWorld();
         }
         ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Edit")) {
         if (ImGui::MenuItem("Duplicate", "CTRL+D")) {
            duplicateActor();
         }
         ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
   }
}

void BIEditorHumanView::showSceneTree()
{
   if (ImGui::Begin("World")) {
      for (const auto& actor : BIEngine::g_pApp->m_pGameLogic->GetActors()) {
         if (actor.second->GetParent() != nullptr) {
            continue;
         }

         showActorTreeNode(actor.second);
      }

      ImGui::End();
   }
}

void BIEditorHumanView::showActorTreeNode(std::shared_ptr<BIEngine::Actor> pActor)
{
   ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_OpenOnArrow;
   const bool isSelected = pActor->GetId() == m_pActorEditorWidget->GetCurrentSelectedActorId();
   if (isSelected) {
      nodeFlags |= ImGuiTreeNodeFlags_Selected;
   }

   if (pActor->GetChildren().size() == 0) {
      nodeFlags |= ImGuiTreeNodeFlags_Leaf;
   }

   const bool isOpened = ImGui::TreeNodeEx((void*)pActor.get(), nodeFlags, pActor->GetName().c_str());

   if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
      m_pActorEditorWidget->SetCurrentEditableActorId(pActor->GetId());
   }

   if (isOpened) {
      for (const auto& child : pActor->GetChildren()) {
         ImGui::Indent(3.0f);
         showActorTreeNode(child);
         ImGui::Unindent(3.0f);
      }
   }

   if (pActor->GetId() == m_pActorEditorWidget->GetCurrentSelectedActorId()) {
      m_pActorEditorWidget->Show();
   }
}

void BIEditorHumanView::saveWorld()
{
   const std::string fileName = "../Assets/" + BIEngine::g_pApp->m_options.mainWorldResNamePath + "/World.xml";

   tinyxml2::XMLDocument worldDoc;
   tinyxml2::XMLElement* const pWorldRootElement = worldDoc.NewElement("World");
   tinyxml2::XMLElement* const pActorsRoot = worldDoc.NewElement("Actors");

   for (const auto& actor : BIEngine::g_pApp->m_pGameLogic->GetActors()) {
      if (actor.second->GetParent() != nullptr) {
         continue;
      }

      tinyxml2::XMLElement* const pActorElement = actor.second->ToXML(&worldDoc);
      pActorsRoot->LinkEndChild(pActorElement);
   }
   pWorldRootElement->LinkEndChild(pActorsRoot);


   tinyxml2::XMLElement* const pScriptElement = worldDoc.NewElement("Script");

   const std::string preLoadScriptPath = BIEngine::g_pApp->m_options.mainWorldResNamePath + "/world_pre_init.py";
   pScriptElement->SetAttribute("preLoad", preLoadScriptPath.c_str());

   const std::string postLoadScriptPath = BIEngine::g_pApp->m_options.mainWorldResNamePath + "/world_POST_init.py";
   pScriptElement->SetAttribute("postLoad", postLoadScriptPath.c_str());

   pWorldRootElement->LinkEndChild(pScriptElement);


   worldDoc.LinkEndChild(pWorldRootElement);

   worldDoc.SaveFile(fileName.c_str());
}

static void editorHumanViewUpdateDuplicateActorName(std::string& name)
{
   std::size_t pos = name.find_last_of('_');
   if (pos == std::string::npos) {
      name += "_0";
      return;
   }

   std::string numStr;
   for (std::size_t i = pos + 1; i < name.size(); ++i) {
      if (name[i] < '0' || name[i] > '9') {
         name += "_0";
         return;
      }

      numStr += name[i];
   }

   if (numStr.size() == 0) {
      name += "_0";
      return;
   }

   int num = std::atoi(numStr.c_str());

   ++num;

   numStr = std::to_string(num);
   name.replace(pos + 1, numStr.size(), numStr.c_str());
}

void BIEditorHumanView::duplicateActor()
{
   const std::shared_ptr<BIEngine::Actor> pActor = BIEngine::g_pApp->m_pGameLogic->GetActor(m_pActorEditorWidget->GetCurrentSelectedActorId());

   tinyxml2::XMLDocument actorXmlDoc;
   tinyxml2::XMLElement* const pActorElement = pActor->ToXML(&actorXmlDoc);
   actorXmlDoc.LinkEndChild(pActorElement);

   const char* name = pActorElement->Attribute("name");
   std::string strName = name;
   editorHumanViewUpdateDuplicateActorName(strName);
   pActorElement->SetAttribute("name", strName.c_str());

   BIEngine::g_pApp->m_pGameLogic->CreateActor(pActorElement);
}
