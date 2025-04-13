#pragma once

#include "../BIEngine/EngineCore/BIEngine.h"

#include "BINetworkManagerClient.h"
#include "BIGameController.h"
#include "BIGCScriptExports.h"
#include "BICameraManager.h"
#include "BIInputActiorController.h"

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
   virtual void OnRender(const BIEngine::GameTimer& gt) override;

private:
   std::unique_ptr<BIInputActionController> m_pInputActionController;
   std::unique_ptr<BICameraManager> m_pCameraManager;

   std::unique_ptr<BINetworkManagerClient> m_pNetworkManager;
   std::shared_ptr<BIGameClientHumanView> m_pHumanView;
};
