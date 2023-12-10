#pragma once

#include "SceneNodes.h"
#include "Model3d.h"
#include "Scene.h"

namespace BIEngine {

// Графический узел, отвечающий за работу с моделями в OpenGL
class Model3dNode : public SceneNode {
public:
   Model3dNode(const ActorId actorId, RenderLayer renderLayer)
      : SceneNode(actorId, renderLayer), m_pModel(nullptr)
   {
   }

   virtual ~Model3dNode() {}

   void SetModel(std::shared_ptr<Model3d> pModel) { m_pModel = pModel; }

   std::shared_ptr<Model3d> GetModel() const { return m_pModel; }

   virtual bool OnRender(Scene* pScene);

protected:
   std::shared_ptr<Model3d> m_pModel;
};

} // namespace BIEngine