#pragma once

#include "../BIEngine/EngineCore/BIEngine.h"

#include "../BIGame/BIFlyCameraSystem.h"

#include "BIEditorController.h"
#include "BIScriptExports.h"

class BIEditorApp : public BIEngine::GameApp {
public:
   explicit BIEditorApp(BIEngine::SharedPtr<BIEngine::GameLogic> pGameLogic);
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

   void SetController(BIEngine::SharedPtr<BIEditorController> controller)
   {
      m_pKeyboardHandler = controller;
      m_pPointerHandler = controller;
   }

private:
   void showMenu();
   void showSceneTree();
   void showActorTreeNode(BIEngine::SharedPtr<BIEngine::Actor> pActor);

   void saveWorld();

   void duplicateActor();

private:
   BIEngine::SharedPtr<BIEngine::Framebuffer> m_pGameRenderTarget;
   BIEngine::SharedPtr<BIEngine::Texture2D> m_pGameRenderTargetColorBuffer;
   BIEngine::SharedPtr<BIEngine::Renderbuffer> m_pGameRenderTargetDepthBuffer;

   BIFlyCameraSystem* m_pFlyCameraSystem;

   ActorEditorWidget* m_pActorEditorWidget;

   BIEngine::WeakPtr<ActorPickerInfoStorage> m_pActorPickerInfoStorage;

   bool m_wasCtrlButtonProcessed;
   bool m_bIsWindowFocused;
};
