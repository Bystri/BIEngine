#pragma once

#include "Scene.h"
#include "RenderItemsStorage.h"

namespace BIEngine {

class IGraphicsTechnique {
public:
   virtual void Init() {}

   virtual void OnRender(Scene* const pScene, RenderItemsStorage* const pStorage) = 0;
};

} // namespace BIEngine
