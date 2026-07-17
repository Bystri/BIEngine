#pragma once

#include "../BIEngine/EngineCore/BIEngine.h"
#include "../BIEngine/Audio/SoundProcess.h"

#include "../BIGame/BIDebugMenuController.h"
#include "../BIGame/BIGameController.h"

#include "BINetworkManagerClient.h"
#include "BIGCScriptExports.h"
#include "BICameraManager.h"
#include "BIInputActiorController.h"

class BIGameClientApp : public BIEngine::GameApp {
public:
   explicit BIGameClientApp(BIEngine::SharedPtr<BIEngine::GameLogic> pGameLogic);
   virtual ~BIGameClientApp();

   virtual bool Init();
   virtual void Close();

   virtual const char* GetGameTitle() override { return "BIGameClient"; }
};

#include <fmod.hpp>

class BIGameClientHumanView : public BIEngine::HumanView {
public:
   BIGameClientHumanView(unsigned int screenWidth, unsigned int screenHeight)
      : BIEngine::HumanView(screenWidth, screenHeight)
   {
   }

   virtual bool Init() override;

private:
   void SetController(BIEngine::SharedPtr<BIGameController> pController)
   {
      m_pKeyboardHandler = pController;
      m_pPointerHandler = pController;
   }

private:
    BIEngine::WeakProcessPtr m_pMainMusicProcess;
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

   BIEngine::SharedPtr<BIGameClientHumanView> m_pHumanView;

#ifndef _RETAIL
   BIEngine::UniquePtr<BIDebugMenuController> m_pDebugMenuController;
#endif
};
