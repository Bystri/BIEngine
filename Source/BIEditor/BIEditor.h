#pragma once

#include "../BIEngine/EngineCore/BIEngine.h"

#include "BIEditorController.h"
#include "BIFlyCameraSystem.h"
#include "BIScriptExports.h"

class BIEditorApp : public BIEngine::GameApp {
public:
   explicit BIEditorApp(std::shared_ptr<BIEngine::GameLogic> pGameLogic);
   virtual ~BIEditorApp();

   virtual bool Init();
   virtual void Close();

   virtual const char* GetGameTitle() override { return "BIEditor"; }
};

class BIEditorLogic : public BIEngine::GameLogic {
public:
   BIEditorLogic();

   virtual bool Init();

   virtual void OnUpdate(BIEngine::GameTimer& gt) override;

   const ActorMap& GetActorMap() { return m_actors; }
};

class ActorEditorWidget;
class ActorPickerInfoStorage;

class BIEditorHumanView : public BIEngine::HumanView {
public:
   BIEditorHumanView(unsigned int screenWidth, unsigned int screenHeight);

   virtual bool Init() override;
   virtual void Shutdown() override;

   virtual void OnUpdate(const BIEngine::GameTimer& gt) override;
   virtual void OnPostRender(const BIEngine::GameTimer& gt) override;

   void SetController(std::shared_ptr<BIEditorController> controller)
   {
      m_pKeyboardHandler = controller;
      m_pPointerHandler = controller;
   }

private:
   void showMenu();
   void showSceneTree();

   void saveWorld();

   void duplicateActor();

private:
   std::shared_ptr<BIEngine::Framebuffer> m_pGameRenderTarget;
   std::shared_ptr<BIEngine::Texture2D> m_pGameRenderTargetColorBuffer;
   std::shared_ptr<BIEngine::Renderbuffer> m_pGameRenderTargetDepthBuffer;

   BIFlyCameraSystem* m_pFlyCameraSystem;

   ActorEditorWidget* m_pActorEditorWidget;

   std::weak_ptr<ActorPickerInfoStorage> m_pActorPickerInfoStorage;

   bool m_wasCtrlButtonProcessed;
   bool m_bIsWindowFocused;
};
