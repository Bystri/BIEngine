#pragma once

#include "../BIEngine/EngineCore/BIEngine.h"
#include "../BIEngine/Utilities/GenericObjectFactory.h"

#include "BIGSScriptExports.h"
#include "BINetworkManagerServer.h"

class BIGameServerApp : public BIEngine::GameApp {
public:
   explicit BIGameServerApp(std::shared_ptr<BIEngine::GameLogic> pGameLogic);
   virtual ~BIGameServerApp();

   virtual bool Init();
   virtual void Close();

   virtual const char* GetGameTitle() override { return "BIGameServer"; }
};

class BIClientView : public BIEngine::HumanView {
public:
   BIClientView()
      : BIEngine::HumanView(0.0f, 0.0f)
   {
   }

   virtual bool Init();
};

class BIServerGameLogic : public BIEngine::GameLogic {
public:
   BIServerGameLogic();

   virtual bool Init() override;
   virtual void Terminate() override;

   virtual bool LoadLevelDelegate(tinyxml2::XMLElement* pRoot) override;

   virtual void OnUpdate(BIEngine::GameTimer& gt) override;

   virtual void OnRender(const BIEngine::GameTimer& gt) override {}

   void PlayerCreatedDelegate(BIEngine::IEventDataPtr pEventData);
   void NewPlayerActorDelegate(BIEngine::IEventDataPtr pEventData);

private:
   std::unique_ptr<BINetworkManagerServer> m_pNetworkManager;
};
