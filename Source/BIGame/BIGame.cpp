#include "BIGame.h"

int main(int argc, char *argv[]) 
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
	m_pPhysics.reset(BIEngine::CreateGamePhysics());
}

bool BIGameLogic::Init()
{
	if (!GameLogic::Init())
		return false;

	m_pPhysics->Initialize();

	std::shared_ptr<BIGameHumanView> pHumanView = std::make_shared<BIGameHumanView>(BIEngine::g_pApp->m_options.screenWidth, BIEngine::g_pApp->m_options.screenHeight);
	AddGameView(pHumanView);

	//Загружаем стартовый мир
	LoadLevel(BIEngine::g_pApp->m_options.mainWorldResName);

	return true;
}

void BIGameLogic::OnUpdate(float dt)
{
	GameLogic::OnUpdate(dt);
}


bool BIGameHumanView::Init()
{
	if (!BIEngine::HumanView::Init())
		return false;
}

void BIGameHumanView::Shutdown()
{
}