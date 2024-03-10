#pragma once

#include "GraphicsTechnique.h"

namespace BIEngine {

class OpaqueGraphicsTechnique : public IGraphicsTechnique {
public:
   OpaqueGraphicsTechnique()
      : IGraphicsTechnique()
   {
   }

   void OnRender(Scene* const pScene, RenderItemsStorage* const pStorage) override;
};

} // namespace BIEngine
