#include "BIGame.h"

#include "../BIEngine/Navigation/NavMeshManager.h"

int main(int argc, char* argv[])
{
   std::shared_ptr<BIGameLogic> pBIGameLogic = std::make_shared<BIGameLogic>();

   BIEngine::g_pApp = new BIGameApp(pBIGameLogic);
   BIEngine::g_pApp->m_options.useDevelopmentAssets = true;
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
   if (!BIEngine::GameApp::Init())
      return false;

   return true;
}

void BIGameApp::Close()
{
   BIEngine::GameApp::Close();
}

BIGameLogic::BIGameLogic()
{
   m_pPhysics2D.reset(BIEngine::CreateGamePhysics2D());
   m_pPhysics3D.reset(BIEngine::CreateGamePhysics3D());

   m_pNavWorld = std::make_unique<BIEngine::NavWorld>();
}

bool BIGameLogic::Init()
{
   if (!GameLogic::Init())
      return false;

   m_pPhysics2D->Initialize();
   m_pPhysics3D->Initialize();

   std::shared_ptr<BIGameHumanView> pHumanView = std::make_shared<BIGameHumanView>(BIEngine::g_pApp->m_options.screenWidth, BIEngine::g_pApp->m_options.screenHeight);
   AddGameView(pHumanView);

   // Загружаем стартовый мир
   LoadLevel(BIEngine::g_pApp->m_options.mainWorldResNamePath);

   return true;
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
   m_pNavWorld->GetNavMeshManager()->RenderMesh();
}

bool BIGameHumanView::Init()
{
   if (!BIEngine::HumanView::Init()) {
      return false;
   }

   std::shared_ptr<BIGameController> pGameController = std::make_shared<BIGameController>();
   SetController(pGameController);

   m_pFlyCameraSystem = new BIFlyCameraSystem(m_pScene->GetCamera(), pGameController);

   return true;
}

void BIGameHumanView::Shutdown()
{
   if (m_pFlyCameraSystem) {
      delete m_pFlyCameraSystem;
      m_pFlyCameraSystem = nullptr;
   }
}

void BIGameHumanView::OnUpdate(const BIEngine::GameTimer& gt)
{
   if (m_pFlyCameraSystem) {
      m_pFlyCameraSystem->OnUpdate(gt);
   }
}
