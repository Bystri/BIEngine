#pragma once

#include "Model.h"
#include "Scene.h"
#include "SceneNodes.h"

namespace BIEngine {

class ModelNode : public SceneNode {
public:
   ModelNode(const ActorId actorId, RenderLayer renderLayer)
      : SceneNode(actorId, renderLayer), m_pModel(nullptr)
   {
   }

   virtual ~ModelNode() {}

   void SetModel(std::shared_ptr<Model> pModel) { m_pModel = pModel; }

   std::shared_ptr<Model> GetModel() const { return m_pModel; }

   virtual bool OnRender(Scene* pScene);

protected:
   std::shared_ptr<Model> m_pModel;
};

} // namespace BIEngine
