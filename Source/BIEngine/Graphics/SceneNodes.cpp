#include "SceneNodes.h"

#include <algorithm>

#include "Model.h"
#include "Scene.h"
#include "RenderItemsStorage.h"
#include "../EngineCore/GameApp.h"
#include "../Renderer/ShadersLoader.h"
#include "../Renderer/Renderbuffer.h"
#include "../Renderer/Color.h"

namespace BIEngine {

glm::mat4 SceneNode::GetLocalModelMatrix() const
{
   if (m_pParent) {
      return m_pParent->GetLocalModelMatrix() * m_props.GetTransformMatrix();
   }

   return m_props.GetTransformMatrix();
}

bool SceneNode::OnUpdate(Scene* pScene, const GameTimer& gt)
{
   for (auto itr = m_children.begin(); itr != m_children.end(); ++itr) {
      (*itr)->OnUpdate(pScene, gt);
   }

   return true;
}

bool SceneNode::PreRender(Scene* pScene)
{
   return true;
}

bool SceneNode::PostRender(Scene* pScene)
{
   return true;
}

bool SceneNode::RenderChildren(Scene* pScene)
{
   for (auto itr = m_children.begin(); itr != m_children.end(); ++itr) {
      if ((*itr)->PreRender(pScene)) {
         if ((*itr)->IsVisible(pScene)) {
            (*itr)->OnRender(pScene);
         }

         (*itr)->RenderChildren(pScene);
      }
      (*itr)->PostRender(pScene);
   }

   return true;
}

void SceneNode::AddChild(std::shared_ptr<ISceneNode> pChild)
{
   m_children.push_back(pChild);

   // TODO: Что насчет, если наш ребенок будет БОЛЬШЕ родителя? Что насчет удаления?
}

void SceneNode::RemoveChild(ActorId id)
{
   for (auto itr = m_children.begin(); itr != m_children.end(); ++itr) {
      const SceneNodeProperties* pProps = (*itr)->Get();

      (*itr)->RemoveChild(id);

      if (id == pProps->GetActorId()) {
         itr = m_children.erase(itr);
      }
   }
}

bool DirectionalLightNode::OnRender(Scene* pScene)
{
   RenderItemsStorage::DirectionalLightItem dirLight;
   dirLight.direction = GetRotation();
   dirLight.color = GetColor();
   dirLight.irradiance = GetIrradiance();

   pScene->GetRenderItemsStorage()->InsertDirectionalLightInfo(dirLight);
   return true;
}

bool PointLightNode::OnRender(Scene* pScene)
{
   RenderItemsStorage::PointLightItem pointLight;
   pointLight.position = GetPosition();

   pointLight.intensity = m_intensity;
   pointLight.color = m_color;

   pScene->GetRenderItemsStorage()->InsertPointLightInfo(pointLight);
   return true;
}

bool SpotLightNode::OnRender(Scene* pScene)
{
   RenderItemsStorage::SpotLightItem spotLight;
   spotLight.position = GetPosition();
   spotLight.direction = GetRotation();

   spotLight.constant = m_attenuationConstant;
   spotLight.linear = m_attenuationLinear;
   spotLight.quadratic = m_attenuationQuadratic;

   spotLight.ambient = GetAmbient();
   spotLight.diffuse = GetDiffuse();
   spotLight.specular = GetSpecular();

   spotLight.cutOff = GetCutOff();
   spotLight.outerCutOff = GetOuterCutOff();

   pScene->GetRenderItemsStorage()->InsertSpotLightInfo(spotLight);
   return true;
}

bool ModelNode::OnRender(Scene* pScene)
{
   const std::vector<std::shared_ptr<ModelMesh>>& modelMeshes = m_pModel->GetMeshes();

   for (const auto& pModelMesh : modelMeshes) {
      RenderItemsStorage::OpaqueRenderItem opaqueRitem;
      opaqueRitem.actorId = m_props.GetActorId();
      opaqueRitem.pMesh = pModelMesh->GetMesh();
      opaqueRitem.pMaterial = pModelMesh->GetMaterial();
      opaqueRitem.ModelTransform = GetLocalModelMatrix();
      pScene->GetRenderItemsStorage()->InsertOpaqueRenderItem(opaqueRitem);
   }

   return true;
}

RootNode::RootNode()
   : SceneNode(Actor::INVALID_ACTOR_ID, RenderLayer::BEGIN)
{
   m_children.reserve(static_cast<size_t>(RenderLayer::END));
   m_children.push_back(std::make_shared<SceneNode>(Actor::INVALID_ACTOR_ID, RenderLayer::LIGHT));
   m_children.push_back(std::make_shared<SceneNode>(Actor::INVALID_ACTOR_ID, RenderLayer::OPAQUE));
}

RootNode::~RootNode()
{
}

void RootNode::AddChild(std::shared_ptr<ISceneNode> pChild)
{
   RenderLayer pass = pChild->Get()->GetRenderLayer();
   if (static_cast<unsigned>(pass) >= m_children.size() || !m_children[static_cast<std::size_t>(pass)]) {
      assert(0 && "There is no such render pass");
      return;
   }

   m_children[static_cast<std::size_t>(pass)]->AddChild(pChild);
}

bool RootNode::RenderChildren(Scene* pScene)
{
   for (int pass = static_cast<unsigned>(RenderLayer::BEGIN); pass < static_cast<unsigned>(RenderLayer::END); ++pass) {
      m_children[pass]->RenderChildren(pScene);
   }

   return true;
}

} // namespace BIEngine
