#include "BIEditor.h"

#include "../BIEngine/Renderer/ShadersLoader.h"
#include "../BIEngine/Renderer/PostProcessor.h"
#include "../BIEngine/Renderer/Renderbuffer.h"

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
   humanView->Init();
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
     m_bIsWindowFocused(false)
{
}

bool BIEditorHumanView::Init()
{
   if (!BIEngine::HumanView::Init()) {
      return false;
   }

   std::shared_ptr<BIEditorController> pGameController = std::make_shared<BIEditorController>();
   SetController(pGameController);

   m_pFlyCameraSystem = new BIFlyCameraSystem(m_pScene->GetCamera(), pGameController);

   m_pGameRenderTarget = std::make_shared<BIEngine::Framebuffer>();
   m_pGameRenderTargetColorBuffer = BIEngine::Texture2D::Create(m_screenWidth, m_screenHeight, BIEngine::Texture::Format::RGB, nullptr);
   m_pGameRenderTargetDepthBuffer = BIEngine::Renderbuffer::Create(m_screenWidth, m_screenHeight, BIEngine::Renderbuffer::Format::DEPTH24);
   FramebufferAttach(m_pGameRenderTarget, BIEngine::FramebufferAttachementType::COLOR, m_pGameRenderTargetColorBuffer);
   FramebufferAttach(m_pGameRenderTarget, BIEngine::FramebufferAttachementType::DEPTH, m_pGameRenderTargetDepthBuffer);
   if (!m_pGameRenderTarget->Check()) {
      return false;
   }

   m_pRenderer->SetRenderTarget(m_pGameRenderTarget);
}

void BIEditorHumanView::Shutdown()
{
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
   BIEngine::HumanView::OnRender(gt);

   ImGui::Begin("Scene");
   {
      ImGui::BeginChild("GameRender");

      float width = ImGui::GetContentRegionAvail().x;
      float height = ImGui::GetContentRegionAvail().y;

      ImGui::Image(
         (ImTextureID)m_pGameRenderTargetColorBuffer->GetId(),
         ImGui::GetContentRegionAvail(),
         ImVec2(0, 1),
         ImVec2(1, 0));

      m_bIsWindowFocused = ImGui::IsWindowFocused();

      ImGui::EndChild();
   }
   ImGui::End();

   BIEngine::GetDefaultFramebuffer()->Bind();
   static const BIEngine::ColorRgba CLEAR_COLOR = BIEngine::ColorRgba(0.0f, 0.5f, 0.5f, 1.0f);
   m_pRenderer->Clear(BIEngine::RenderDevice::ClearFlag::COLOR | BIEngine::RenderDevice::ClearFlag::DEPTH, CLEAR_COLOR);
}
