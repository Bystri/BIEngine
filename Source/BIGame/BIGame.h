#pragma once

#include "../BIEngine/EngineCore/BIEngine.h"

#include "BIScriptExports.h"
#include "BICameraManager.h"

class BIGameApp : public BIEngine::GameApp {
public:
   explicit BIGameApp(std::shared_ptr<BIEngine::GameLogic> pGameLogic);
   virtual ~BIGameApp();

   virtual bool Init();
   virtual void Close();

   virtual const char* GetGameTitle() override { return "BIGame"; }
};

class BIGameHumanView : public BIEngine::HumanView {
public:
   BIGameHumanView(unsigned int screenWidth, unsigned int screenHeight)
      : BIEngine::HumanView(screenWidth, screenHeight)
   {
   }

   virtual bool Init();
};

class BIGameLogic : public BIEngine::GameLogic {
public:
   BIGameLogic();

   virtual bool Init() override;
   virtual void Terminate() override;

   virtual bool LoadLevelDelegate(tinyxml2::XMLElement* pRoot) override;

   void NewPlayerActorDelegate(BIEngine::IEventDataPtr pEventData);

   virtual void OnUpdate(BIEngine::GameTimer& gt) override;
   virtual void OnRender(const BIEngine::GameTimer& gt) override;

private:
   std::unique_ptr<BICameraManager> m_pCameraManager;
   std::shared_ptr<BIGameHumanView> m_pHumanView;
};
