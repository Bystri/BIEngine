#pragma once

#include "../BIEngine/EngineCore/BIEngine.h"

#include "BINetworkManagerClient.h"
#include "BIGameController.h"
#include "BIGCScriptExports.h"
#include "BICameraManager.h"
#include "BIInputActiorController.h"
#include "BIDebugMenuController.h"

class BIGameClientApp : public BIEngine::GameApp {
public:
   explicit BIGameClientApp(std::shared_ptr<BIEngine::GameLogic> pGameLogic);
   virtual ~BIGameClientApp();

   virtual bool Init();
   virtual void Close();

   virtual const char* GetGameTitle() override { return "BIGameClient"; }
};

class BIGameClientHumanView : public BIEngine::HumanView {
public:
   BIGameClientHumanView(unsigned int screenWidth, unsigned int screenHeight)
      : BIEngine::HumanView(screenWidth, screenHeight)
   {
   }

   virtual bool Init();

private:
   void SetController(std::shared_ptr<BIGameController> pController)
   {
      m_pKeyboardHandler = pController;
      m_pPointerHandler = pController;
   }
};

class BIGameClientLogic : public BIEngine::GameLogic {
public:
   BIGameClientLogic();

   virtual bool Init() override;
   virtual void Terminate() override;

   virtual bool LoadLevelDelegate(tinyxml2::XMLElement* pRoot) override;

   void NewPlayerActorDelegate(BIEngine::IEventDataPtr pEventData);

   virtual void OnUpdate(BIEngine::GameTimer& gt) override;
   virtual void OnRenderDebug(const BIEngine::GameTimer& gt) override;

private:
   BIEngine::EventManager::DelegateHandler m_newPlayerActorDelegateHandler;

   BIEngine::UniquePtr<BIInputActionController> m_pInputActionController;
   BIEngine::UniquePtr<BICameraManager> m_pCameraManager;

   BIEngine::UniquePtr<BINetworkManagerClient> m_pNetworkManager;
   std::shared_ptr<BIGameClientHumanView> m_pHumanView;

   BIEngine::UniquePtr<BIDebugMenuController> m_pDebugMenuController;
};
