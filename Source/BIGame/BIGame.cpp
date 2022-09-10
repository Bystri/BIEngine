#include "BIGame.h"

#include <glm/glm.hpp>

#include "../BIEngine/ResourceCache/ResCache.h"
#include "../BIEngine/Graphics2D/SceneNodes.h"
#include "../BIEngine/ResourceCache/XmlLoader.h"
#include "../BIEngine/Actors/TransformComponent.h"
#include "../BIEngine/Actors/PhysicsComponent.h"
#include "../BIEngine/Actors/PhysicsTriggerComponent.h"
#include "../BIEngine/Physics/PhysicsEventListener.h"
#include "BIGameController.h"
#include "BIGameEvents.h"

int main(int argc, char *argv[]) 
{
	std::shared_ptr<BIGameLogic> pBIGameLogic = std::make_shared<BIGameLogic>();

	BIEngine::g_pApp = new BIGameApp(pBIGameLogic);
	g_pApp->m_options.useDevelopmentAssets = true;
	return BIEngine::Run(argc, argv);
}


const float INITIAL_Y_BALL_SPEED = 350.0;

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
	m_player1Points = 0;
	m_player2Points = 0;
	m_ballSpeedMultiplay = 1.0f;
}

bool BIGameLogic::Init()
{
	if (!GameLogic::Init())
		return false;

	m_pPhysics->Initialize();

	std::shared_ptr<BIGameHumanView> pHumanView = std::make_shared<BIGameHumanView>(g_pApp->m_options.screenWidth, g_pApp->m_options.screenHeight);
	pHumanView->Init();

	std::shared_ptr<BIEngine::XmlExtraData> pCameraData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle("actors/camera.xml")->GetExtra());
	auto cameraActor = CreateActor(pCameraData->GetRootElement());

	std::shared_ptr<BIEngine::XmlExtraData> pBackgroundData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle("actors/background.xml")->GetExtra());
	std::shared_ptr<BIEngine::Actor> pBackground = CreateActor(pBackgroundData->GetRootElement());

	std::shared_ptr<BIEngine::XmlExtraData> pBorderData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle("actors/border.xml")->GetExtra());
	std::shared_ptr<BIEngine::Actor> pBorder1 = CreateActor(pBorderData->GetRootElement());
	auto b1Transofrm = pBorder1->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
	b1Transofrm->SetPosition(0, 340);
	std::shared_ptr<BIEngine::Actor> pBorder2 = CreateActor(pBorderData->GetRootElement());
	auto b2Transofrm = pBorder2->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
	b2Transofrm->SetPosition(360, 340);

	std::shared_ptr<BIEngine::XmlExtraData> pPaddleData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle("actors/paddle.xml")->GetExtra());
	m_pPlayer1 = CreateActor(pPaddleData->GetRootElement());
	m_pPlayer2 = CreateActor(pPaddleData->GetRootElement());

	auto p1Transofrm = m_pPlayer1->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
	p1Transofrm->SetPosition(180, 60);

	auto p2Transofrm = m_pPlayer2->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
	p2Transofrm->SetPosition(180, 580);

	std::shared_ptr<BIEngine::XmlExtraData> pBallData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle("actors/ball.xml")->GetExtra());
	m_pBall = CreateActor(pBallData->GetRootElement());
	auto ballTransofrm = m_pBall->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
	ballTransofrm->SetPosition(180, 320);

	std::shared_ptr<BIEngine::XmlExtraData> pTrigData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle("actors/point_trigger.xml")->GetExtra());
	m_pPlayer1Trigger = CreateActor(pTrigData->GetRootElement());
	auto t1Transofrm = m_pPlayer1Trigger->GetComponent<BIEngine::PhysicsTriggerComponent>(BIEngine::PhysicsTriggerComponent::g_CompId).lock();
	t1Transofrm->SetPosition(180, 30);
	m_pPlayer2Trigger = CreateActor(pTrigData->GetRootElement());
	auto t2Transofrm = m_pPlayer2Trigger->GetComponent<BIEngine::PhysicsTriggerComponent>(BIEngine::PhysicsTriggerComponent::g_CompId).lock();
	t2Transofrm->SetPosition(180, 610);

	std::shared_ptr<BIGameController> pController = std::make_shared<BIGameController>(m_pPlayer1->GetId(), m_pPlayer2->GetId());
	pHumanView->SetController(pController);
	AddGameView(pHumanView);

	m_ballSpeed = glm::vec2(0, INITIAL_Y_BALL_SPEED);

	BIEngine::EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BIGameLogic::ChangeBallDirection), BIEngine::EvtData_PhysCollision::sk_EventType);
	BIEngine::EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BIGameLogic::AddPoint), BIEngine::EvtData_PhysTrigger_Enter::sk_EventType);
	BIEngine::EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BIGameLogic::MovePaddle), EvtData_StartPaddleMove::sk_EventType);
	BIEngine::EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BIGameLogic::StopPaddle), EvtData_EndPaddleMove::sk_EventType);

	return true;
}

void BIGameLogic::OnUpdate(float dt)
{
	GameLogic::OnUpdate(dt);

	auto physComp = m_pBall->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
	physComp->SetVelocity(m_ballSpeed);
}

void BIGameLogic::ChangeBallDirection(BIEngine::IEventDataPtr pEventData)
{
	std::shared_ptr<EvtData_PhysCollision> pData = std::static_pointer_cast<EvtData_PhysCollision>(pEventData);
	
	if (pData->GetActorA() == m_pBall->GetId() || pData->GetActorB() == m_pBall->GetId())
	{
		if (pData->GetActorA() == m_pPlayer1->GetId() || pData->GetActorB() == m_pPlayer1->GetId())
		{
			m_ballSpeed.y *= -1;
			auto pTransofrm = m_pPlayer1->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
			m_ballSpeed.x = pTransofrm->GetVelocity().x;
		}
		else if (pData->GetActorA() == m_pPlayer2->GetId() || pData->GetActorB() == m_pPlayer2->GetId())
		{
			m_ballSpeed.y *= -1;
			auto pTransofrm = m_pPlayer2->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
			m_ballSpeed.x = pTransofrm->GetVelocity().x;
		}
		else 
		{
			m_ballSpeed.x *= -1;
		}
	}
	else if (pData->GetActorA() == m_pPlayer1->GetId() || pData->GetActorB() == m_pPlayer1->GetId())
	{
		auto physComp = m_pPlayer1->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
		physComp->Stop();
	}
	else
	{
		auto physComp = m_pPlayer2->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
		physComp->Stop();
	}
}

void BIGameLogic::AddPoint(BIEngine::IEventDataPtr pEventData)
{
	std::shared_ptr<EvtData_PhysTrigger_Enter> pData = std::static_pointer_cast<EvtData_PhysTrigger_Enter>(pEventData);

	if (pData->GetOtherActor() == m_pBall->GetId())
	{
		if (pData->GetTriggerId() == m_pPlayer1Trigger->GetId())
			m_player2Points++;

		if (pData->GetTriggerId() == m_pPlayer2Trigger->GetId())
			m_player1Points++;

		auto ballTransofrm = m_pBall->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
		ballTransofrm->SetPosition(180, 320);

		m_ballSpeed = glm::vec2(0, INITIAL_Y_BALL_SPEED);

		std::shared_ptr<EvtData_UpdateUI> pEventData = std::make_shared<EvtData_UpdateUI>(m_player1Points, m_player2Points);
		EventManager::Get()->QueueEvent(pEventData);
	}
}

void BIGameLogic::MovePaddle(BIEngine::IEventDataPtr pEventData)
{
	std::shared_ptr<EvtData_StartPaddleMove> pEventPaddleData = std::dynamic_pointer_cast<EvtData_StartPaddleMove>(pEventData);
	if (pEventPaddleData->GetActorId() == m_pPlayer1->GetId())
	{
		auto physComp = m_pPlayer1->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
		physComp->SetVelocity(glm::vec2(pEventPaddleData->GetVelocity(), 0.0f));
	} 
	else
	{
		auto physComp = m_pPlayer2->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
		physComp->SetVelocity(glm::vec2(pEventPaddleData->GetVelocity(), 0.0f));
	}
}

void BIGameLogic::StopPaddle(BIEngine::IEventDataPtr pEventData)
{
	std::shared_ptr<EvtData_EndPaddleMove> pEventPaddleData = std::dynamic_pointer_cast<EvtData_EndPaddleMove>(pEventData);
	if (pEventPaddleData->GetActorId() == m_pPlayer1->GetId())
	{
		auto physComp = m_pPlayer1->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
		physComp->Stop();
	}
	else
	{
		auto physComp = m_pPlayer2->GetComponent<BIEngine::PhysicsComponent>(BIEngine::PhysicsComponent::g_CompId).lock();
		physComp->Stop();
	}
}


bool BIGameHumanView::Init()
{
	if (!BIEngine::HumanView::Init())
		return false;

	//Загружаем и запускаем музыку
	std::shared_ptr<BIEngine::ResHandle> pAudioResHandle = BIEngine::ResCache::Get()->GetHandle("Sounds/Space.ogg");
	BIEngine::IAudioBuffer* buffer = BIEngine::g_pAudio->InitAudioBuffer(pAudioResHandle);
	if (buffer)
	{
		m_pMainMusic.reset(buffer);
		m_pMainMusic->Play(100, true);
	}

	//Устанавливаем на экране текст
	m_userInterface.CreateStatic(10.f, 20.0f, 1.0f, "Player1 : 0");
	m_userInterface.CreateStatic(240.f, 20.0f, 1.0f, "Player2 : 0");

	RegisterAllDelegates();
}

void BIGameHumanView::Shutdown()
{
	RemoveAllDelegates();
}

void BIGameHumanView::UpdateUI(BIEngine::IEventDataPtr pEventData)
{
	std::shared_ptr<EvtData_UpdateUI> pEventUIData = std::dynamic_pointer_cast<EvtData_UpdateUI>(pEventData);
	m_userInterface.GetStatic(0)->SetText("Player1 : " + std::to_string(pEventUIData->GetPlayer1Points()));
	m_userInterface.GetStatic(1)->SetText("Player2 : " + std::to_string(pEventUIData->GetPlayer2Points()));
}

void BIGameHumanView::RegisterAllDelegates()
{
	BIEngine::EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &BIGameHumanView::UpdateUI), EvtData_UpdateUI::sk_EventType);
}

void BIGameHumanView::RemoveAllDelegates()
{
	BIEngine::EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &BIGameHumanView::UpdateUI), EvtData_UpdateUI::sk_EventType);
}

