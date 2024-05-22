#pragma once

#include "../BIEngine/EngineCore/GameApp.h"
#include "../BIEngine/EngineCore/GameLogic.h"
#include "BIEditorController.h"

class BIEditorApp : public BIEngine::GameApp
{
public:
    explicit BIEditorApp(std::shared_ptr<BIEngine::GameLogic> pGameLogic);
    virtual ~BIEditorApp();

    virtual bool Init();
    virtual void Close();

    virtual const char* GetGameTitle() override { return "PingPong"; }
};


class BIEditorLogic : public BIEngine::GameLogic
{
public:
    BIEditorLogic();

    virtual bool Init();

    virtual void OnUpdate(float dt) override;

    const ActorMap& GetActorMap() { return m_actors; }
};


class BIEditorHumanView : public BIEngine::HumanView
{
public:
    BIEditorHumanView(unsigned int screenWidth, unsigned int screenHeight);

    virtual bool Init();
    virtual void Shutdown();

    void SetController(std::shared_ptr<BIEditorController> controller)
    {
        m_pKeyboardHandler = controller;
        m_pPointerHandler = controller;
    }

protected:
    virtual void NewCameraComponentDelegate(BIEngine::IEventDataPtr pEventData);

private:
    void RegisterAllDelegates();
    void RemoveAllDelegates();

};
