#include "BIEditor.h"

#include <glm/glm.hpp>

#include "../BIEngine/ResourceCache/ResCache.h"
#include "../BIEngine/Graphics2D/SceneNodes.h"
#include "../BIEngine/ResourceCache/XmlLoader.h"
#include "../BIEngine/Actors/TransformComponent.h"
#include "../BIEngine/Actors/PhysicsComponent.h"
#include "../BIEngine/Actors/PhysicsTriggerComponent.h"
#include "../BIEngine/Physics/PhysicsEventListener.h"

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
    if (!BIEngine::GameApp::Init())
        return false;

    return true;
}

void BIEditorApp::Close()
{
    BIEngine::GameApp::Close();
}

BIEditorLogic::BIEditorLogic()
{
    m_pPhysics.reset(BIEngine::CreateNullPhysics());
}

bool BIEditorLogic::Init()
{
    if (!GameLogic::Init())
        return false;

    m_pPhysics->Initialize();

    std::shared_ptr<BIEditorHumanView> humanView = std::make_shared<BIEditorHumanView>(BIEngine::g_pApp->m_options.screenWidth, BIEngine::g_pApp->m_options.screenHeight);
    humanView->Init();

    std::shared_ptr<BIEditorController> controller = std::make_shared<BIEditorController>(nullptr);
    humanView->SetController(controller);
    AddGameView(humanView);

    return true;
}

void BIEditorLogic::OnUpdate(float dt)
{
    GameLogic::OnUpdate(dt);
}

BIEditorHumanView::BIEditorHumanView(unsigned int screenWidth, unsigned int screenHeight)
    : BIEngine::HumanView(screenWidth, screenHeight)
{

}

bool BIEditorHumanView::Init()
{
    if (!BIEngine::HumanView::Init())
        return false;


    RegisterAllDelegates();
}

void BIEditorHumanView::Shutdown()
{
    RemoveAllDelegates();
}

void BIEditorHumanView::NewCameraComponentDelegate(BIEngine::IEventDataPtr pEventData)
{
    HumanView::NewCameraComponentDelegate(pEventData);

    std::shared_ptr<BIEditorController> controller = std::make_shared<BIEditorController>(m_pCameraActor);
    SetController(controller);
}

void BIEditorHumanView::RegisterAllDelegates()
{
}

void BIEditorHumanView::RemoveAllDelegates()
{
}

