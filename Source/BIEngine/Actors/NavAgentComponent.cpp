#include "NavAgentComponent.h"

#include "../Actors/TransformComponent.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine {

const ComponentId NavAgentComponent::g_CompId = "NavAgentComponent";

NavAgentComponent::NavAgentComponent()
   : m_pNavCrowd(nullptr), m_agentId(-1)
{
}

NavAgentComponent::~NavAgentComponent()
{
   if (m_agentId != -1) {
      m_pNavCrowd->RemoveAgent(m_agentId);
   }
}

bool NavAgentComponent::Init(tinyxml2::XMLElement* pData)
{
   tinyxml2::XMLElement* pCommonSettings = pData->FirstChildElement("Common");
   if (pCommonSettings) {
      float radius = 0.5f;
      float height = 1.8f;
      float maxAcceleration = 8.0f;
      float maxSpeed = 3.5f;

      pCommonSettings->QueryFloatAttribute("radius", &radius);
      pCommonSettings->QueryFloatAttribute("height", &height);
      pCommonSettings->QueryFloatAttribute("maxAcceleration", &maxAcceleration);
      pCommonSettings->QueryFloatAttribute("maxSpeed", &maxSpeed);

      m_params.Radius = radius;
      m_params.Height = height;
      m_params.MaxAcceleration = maxAcceleration;
      m_params.MaxSpeed = maxSpeed;
   }

   tinyxml2::XMLElement* pSteering = pData->FirstChildElement("Steering");
   if (pSteering) {
      bool anticipateTurns = false;

      pSteering->QueryBoolAttribute("anticipateTurns", &anticipateTurns);

      m_params.AnticipateTurns = anticipateTurns;
   }

   tinyxml2::XMLElement* pSeparationSettings = pData->FirstChildElement("Separation");
   if (pSeparationSettings) {
      bool enable = false;
      float separationWeight = 5.0f;

      pSeparationSettings->QueryBoolAttribute("enable", &enable);
      pSeparationSettings->QueryFloatAttribute("separationWeight", &separationWeight);

      m_params.Separation = enable;
      m_params.SeparationWeight = separationWeight;
   }

   tinyxml2::XMLElement* pObstacleAvoidance = pData->FirstChildElement("ObstacleAvoidance");
   if (pObstacleAvoidance) {
      bool enable = false;
      float collisionQueryRange = 6.0f;

      pObstacleAvoidance->QueryBoolAttribute("enable", &enable);
      pObstacleAvoidance->QueryFloatAttribute("collisionQueryRange", &collisionQueryRange);

      m_params.ObstacleAvoidance = enable;
      m_params.CollisionQueryRange = collisionQueryRange;
   }

   tinyxml2::XMLElement* pPathOptimizations = pData->FirstChildElement("PathOptimizations");
   if (pPathOptimizations) {
      float pathOptimizationRange = 15.0f;
      bool optimizeVis = false;
      bool optimizeTopo = false;

      pPathOptimizations->QueryBoolAttribute("optimizeVis", &optimizeVis);
      pPathOptimizations->QueryFloatAttribute("pathOptimizationRange", &pathOptimizationRange);
      pPathOptimizations->QueryBoolAttribute("optimizeTopo", &optimizeTopo);

      m_params.OptimizeVis = optimizeVis;
      m_params.PathOptimizationRange = pathOptimizationRange;
      m_params.OptimizeTopo = optimizeTopo;
   }

   return true;
}

void NavAgentComponent::Activate()
{
   if (!g_pApp->m_pGameLogic->GetNavWorld()) {
      return;
   }

   m_pNavCrowd = g_pApp->m_pGameLogic->GetNavWorld()->GetNavCrowd();

   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   m_agentId = m_pNavCrowd->AddAgent(m_pOwner->GetId(), pTransformComponent->GetPosition(), m_params);
}

tinyxml2::XMLElement* NavAgentComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   tinyxml2::XMLElement* pCommonSettings = pDoc->NewElement("Common");
   pCommonSettings->SetAttribute("radius", m_params.Radius);
   pCommonSettings->SetAttribute("height", m_params.Height);
   pCommonSettings->SetAttribute("maxSpeed", m_params.MaxSpeed);
   pCommonSettings->SetAttribute("maxAcceleration", m_params.MaxAcceleration);
   pBaseElement->LinkEndChild(pCommonSettings);

   tinyxml2::XMLElement* pSteering = pDoc->NewElement("Steering");
   pSteering->SetAttribute("anticipateTurns", m_params.AnticipateTurns);
   pBaseElement->LinkEndChild(pSteering);

   tinyxml2::XMLElement* pSeparationSettings = pDoc->NewElement("Separation");
   pSeparationSettings->SetAttribute("enable", m_params.Separation);
   pSeparationSettings->SetAttribute("separationWeight", m_params.SeparationWeight);
   pBaseElement->LinkEndChild(pSeparationSettings);

   tinyxml2::XMLElement* pObstacleAvoidance = pDoc->NewElement("ObstacleAvoidance");
   pObstacleAvoidance->SetAttribute("enable", m_params.ObstacleAvoidance);
   pObstacleAvoidance->SetAttribute("collisionQueryRange", m_params.CollisionQueryRange);
   pBaseElement->LinkEndChild(pObstacleAvoidance);


   tinyxml2::XMLElement* pPathOptimizations = pDoc->NewElement("PathOptimizations");
   pPathOptimizations->SetAttribute("optimizeVis", m_params.OptimizeVis);
   pPathOptimizations->SetAttribute("pathOptimizationRange", m_params.PathOptimizationRange);
   pPathOptimizations->SetAttribute("optimizeTopo", m_params.OptimizeTopo);
   pBaseElement->LinkEndChild(pPathOptimizations);

   return pBaseElement;
}

bool NavAgentComponent::SetDestination(const glm::vec3& pos)
{
   if (m_agentId == -1) {
      return false;
   }

   return m_pNavCrowd->SetDestination(m_agentId, pos);
}

glm::vec3 NavAgentComponent::GetVelocity() const
{
   if (m_agentId == -1) {
      return glm::vec3(0.0f);
   }

   return m_pNavCrowd->GetVelocity(m_agentId);
}

} // namespace BIEngine
