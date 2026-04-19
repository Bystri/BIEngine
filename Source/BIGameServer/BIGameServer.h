#pragma once

#include "../BIEngine/EngineCore/BIEngine.h"
#include "../BIEngine/Utilities/GenericObjectFactory.h"

#include "../BIGame/BIDebugMenuController.h"
#include "../BIGame/BIFlyCameraSystem.h"

#include "BIGSScriptExports.h"
#include "BINetworkManagerServer.h"

class BIGameServerApp : public BIEngine::GameApp {
public:
   explicit BIGameServerApp(BIEngine::SharedPtr<BIEngine::GameLogic> pGameLogic);
   virtual ~BIGameServerApp();

   virtual bool Init();
   virtual void Close();

   virtual const char* GetGameTitle() override { return "BIGameServer"; }
};

#ifndef _RETAIL

class BIServerDbgHumanView : public BIEngine::HumanView {
public:
   BIServerDbgHumanView(unsigned int screenWidth, unsigned int screenHeight)
      : BIEngine::HumanView(screenWidth, screenHeight)
   {
   }

   virtual bool Init() override;

   virtual void OnUpdate(const BIEngine::GameTimer& gt) override;

private:
   void SetController(BIEngine::SharedPtr<BIGameController> pController)
   {
      m_pKeyboardHandler = pController;
      m_pPointerHandler = pController;
   }

private:
#ifndef _RETAIL
   BIEngine::UniquePtr<BIFlyCameraSystem> m_pFlyCameraSystem;
#endif
};

#endif

class BIServerGameLogic : public BIEngine::GameLogic {
public:
   BIServerGameLogic();

   virtual bool Init() override;
   virtual void Terminate() override;

   virtual bool LoadLevelDelegate(tinyxml2::XMLElement* pRoot) override;

   virtual void OnUpdate(BIEngine::GameTimer& gt) override;
   virtual void OnRenderDebug(const BIEngine::GameTimer& gt) override;

   void OnNetPeerConnectedDelegate(BIEngine::IEventDataPtr pEventData);
   void OnNetPeerDisonnectedDelegate(BIEngine::IEventDataPtr pEventData);

private:
   BIEngine::EventManager::DelegateHandler m_netPeerConnectedDelegateHandler;
   BIEngine::EventManager::DelegateHandler m_netPeerDisonnectedDelegateHandler;

   BIEngine::HashMap<BIEngine::PeerId, BIEngine::SharedPtr<ReplicationObjectPlayer>> m_peerIdToPlayerMap;

#ifndef _RETAIL
   BIEngine::SharedPtr<BIServerDbgHumanView> m_pDbgHumanView;
   BIEngine::UniquePtr<BIDebugMenuController> m_pDebugMenuController;
#endif
};
