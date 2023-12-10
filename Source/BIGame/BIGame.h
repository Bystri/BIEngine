#pragma once

#include "../BIEngine/EngineCore/BIEngine.h"
#include "BIGameController.h"
#include "BIFlyCameraSystem.h"

class BIGameApp : public BIEngine::GameApp {
public:
   explicit BIGameApp(std::shared_ptr<BIEngine::GameLogic> pGameLogic);
   virtual ~BIGameApp();

   virtual bool Init();
   virtual void Close();

   virtual const char* GetGameTitle() override { return "BIGame"; }
};

class BIGameLogic : public BIEngine::GameLogic {
public:
   BIGameLogic();

   virtual bool Init();

   virtual void OnUpdate(float dt) override;
};

class BIGameHumanView : public BIEngine::HumanView {
public:
   BIGameHumanView(unsigned int screenWidth, unsigned int screenHeight)
      : BIEngine::HumanView(screenWidth, screenHeight), m_pFlyCameraSystem(nullptr)
   {
   }

   virtual bool Init();
   virtual void Shutdown();

   virtual void OnUpdate(float dt) override;

private:
   void SetController(std::shared_ptr<BIGameController> pController)
   {
      m_pKeyboardHandler = pController;
      m_pPointerHandler = pController;
   }

private:
   BIFlyCameraSystem* m_pFlyCameraSystem;
};