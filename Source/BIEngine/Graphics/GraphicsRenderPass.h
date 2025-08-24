#pragma once

#include "../StdLib/SharedPtr.h"
#include "GraphicsTechnique.h"
#include "RenderItemsStorage.h"

namespace BIEngine {

class GraphicsRenderPass {
public:
   GraphicsRenderPass()
      : m_graphicsTechniques()
   {
   }

   bool Init();

   void AddTechnique(SharedPtr<IGraphicsTechnique> pTechnique);

   void OnRender(Scene* const pScene, RenderItemsStorage* const pStorage);

protected:
   virtual bool InitInternal() { return true; }

   virtual void PreRender(Scene* const pScene) {}

   virtual void PostRender(Scene* const pScene) {}

private:
   DynamicArray<SharedPtr<IGraphicsTechnique>> m_graphicsTechniques;
};

} // namespace BIEngine
