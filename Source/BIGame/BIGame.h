#pragma once

#include "../BIEngine/EngineCore/BIEngine.h"
#include "../BIEngine/StdLib/SharedPtr.h"

#include "BIScriptExports.h"
#include "BICameraManager.h"

class BIGameApp : public BIEngine::GameApp {
public:
   explicit BIGameApp(BIEngine::SharedPtr<BIEngine::GameLogic> pGameLogic);
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

   virtual void OnUpdate(BIEngine::GameTimer& gt) override;

private:
   BIEngine::UniquePtr<BICameraManager> m_pCameraManager;
   BIEngine::SharedPtr<BIGameHumanView> m_pHumanView;
};
