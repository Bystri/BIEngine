#pragma once

#include <memory>

#include "../Widgets/ActorEditorWidget.h"
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

   std::shared_ptr<BIEngine::ShaderProgram> m_pEmptyColorSp;
   std::shared_ptr<BIEngine::ShaderProgram> m_pSolidColorSp;
};
