#pragma once

#include "../Widgets/ActorEditorWidget.h"
#include "../../BIEngine/StdLib/SharedPtr.h"
#include "../../BIEngine/Renderer/ShadersLoader.h"
#include "../../BIEngine/Graphics/GraphicsTechnique.h"

class SelectedActorOutliner : public BIEngine::IGraphicsTechnique {
public:
   SelectedActorOutliner(ActorEditorWidget* pActorEditorWidget)
      : m_pActorEditorWidget(pActorEditorWidget)
   {
   }

   bool Init() override;

   void OnRender(BIEngine::Scene* const pScene, BIEngine::RenderItemsStorage* const pStorage);

private:
   ActorEditorWidget* m_pActorEditorWidget;

   BIEngine::SharedPtr<BIEngine::ShaderProgram> m_pEmptyColorSp;
   BIEngine::SharedPtr<BIEngine::ShaderProgram> m_pSolidColorSp;
};
