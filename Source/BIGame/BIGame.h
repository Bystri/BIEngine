#pragma once

#include "../BIEngine/EngineCore/BIEngine.h"
#include "../BIEngine/Audio/irrKlangAudio.h"
#include "BIGameController.h"

class BIGameApp : public BIEngine::GameApp
{
public:
	explicit BIGameApp(std::shared_ptr<BIEngine::GameLogic> pGameLogic);
	virtual ~BIGameApp();

	virtual bool Init();
	virtual void Close();

	virtual const char* GetGameTitle() override { return "PingPong"; }
};


class BIGameLogic : public BIEngine::GameLogic
{
public:
	BIGameLogic();

	virtual bool Init();

	virtual void OnUpdate(float dt) override;

	void ChangeBallDirection(BIEngine::IEventDataPtr pEventData);
	void AddPoint(BIEngine::IEventDataPtr pEventData);

	void MovePaddle(BIEngine::IEventDataPtr pEventData);
	void StopPaddle(BIEngine::IEventDataPtr pEventData);

private:
	glm::vec2 m_ballSpeed;
	float m_ballSpeedMultiplay;

	std::shared_ptr<BIEngine::Actor> m_pPlayer1;
	std::shared_ptr<BIEngine::Actor> m_pPlayer2;
	std::shared_ptr<BIEngine::Actor> m_pPlayer1Trigger;
	std::shared_ptr<BIEngine::Actor> m_pPlayer2Trigger;
	std::shared_ptr<BIEngine::Actor> m_pBall;

	unsigned int m_player1Points;
	unsigned int m_player2Points;
};


class BIGameHumanView : public BIEngine::HumanView
{
public:
	BIGameHumanView(unsigned int screenWidth, unsigned int screenHeight)
		: BIEngine::HumanView(screenWidth, screenHeight)
	{

	}

	virtual bool Init();
	virtual void Shutdown();

	void SetController(std::shared_ptr<BIGameController> pController)
	{
		m_pKeyboardHandler = pController;
		m_pPointerHandler = pController;
	}

	void UpdateUI(BIEngine::IEventDataPtr pEventData);

private:
	void RegisterAllDelegates();
	void RemoveAllDelegates();

private:
	std::shared_ptr <BIEngine::IAudioBuffer> m_pMainMusic;
};