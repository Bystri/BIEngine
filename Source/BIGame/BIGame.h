#pragma once

#include "../BIEngine/EngineCore/BIEngine.h"
#include "../BIEngine/Network/NetworkManagerP2P.h"

#include "BIGameController.h"
#include "BIScriptExports.h"

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

   virtual bool LoadLevelDelegate(tinyxml2::XMLElement* pRoot) override;

   virtual void OnUpdate(BIEngine::GameTimer& gt) override;

private:
   std::unique_ptr<BIEngine::NetworkManager> m_pNetworkManager;
};

class BIGameHumanView : public BIEngine::HumanView {
public:
   BIGameHumanView(unsigned int screenWidth, unsigned int screenHeight)
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
