#include "Physics3DComponent.h"

#include "TransformComponent.h"
#include "../Physics/Physics3D.h"
#include "../Utilities/String.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine {

const ComponentId Physics3DComponent::g_CompId = "Physics3DComponent";

Physics3DComponent::Physics3DComponent()
{
   m_rigidBodyScale = glm::vec3(1.f, 1.f, 1.f);

   m_shape = Shape3D::NONE;
   m_bodyType = IGamePhysics3D::BodyType::DYNAMIC;
}

bool Physics3DComponent::Init(tinyxml2::XMLElement* pData)
{
   m_gamePhysics = g_pApp->m_pGameLogic->GetGamePhysics3D();
   if (!m_gamePhysics) {
      return false;
   }

   tinyxml2::XMLElement* pShape = pData->FirstChildElement("Shape");
   if (pShape) {
      std::string shapeStr = pShape->FirstChild()->Value();
      if (shapeStr == "Sphere")
         m_shape = Shape3D::SPHERE;
      else if (shapeStr == "Box")
         m_shape = Shape3D::BOX;
      else if (shapeStr == "Capsule")
         m_shape = Shape3D::CAPSULE;
      else if (shapeStr == "PointCloud")
         m_shape = Shape3D::POINT_CLOUD;
   }

   tinyxml2::XMLElement* pBodyType = pData->FirstChildElement("BodyType");
   if (pBodyType) {
      std::string typeStr = pBodyType->FirstChild()->Value();
      if (typeStr == "Dynamic")
         m_bodyType = IGamePhysics3D::BodyType::DYNAMIC;
      else if (typeStr == "Kinematic")
         m_bodyType = IGamePhysics3D::BodyType::KINEMATIC;
      else if (typeStr == "Static")
         m_bodyType = IGamePhysics3D::BodyType::STATIC;
   }

   tinyxml2::XMLElement* pDensity = pData->FirstChildElement("Density");
   if (pDensity) {
      m_density = pDensity->FirstChild()->Value();
   }

   tinyxml2::XMLElement* pMaterial = pData->FirstChildElement("PhysicsMaterial");
   if (pMaterial) {
      m_material = pMaterial->FirstChild()->Value();
   }

   tinyxml2::XMLElement* pSizeElement = pData->FirstChildElement("Scale");
   if (pSizeElement) {
      double w = 0;
      double h = 0;
      double d = 0;
      pSizeElement->QueryDoubleAttribute("w", &w);
      pSizeElement->QueryDoubleAttribute("h", &h);
      pSizeElement->QueryDoubleAttribute("d", &d);
      m_rigidBodyScale = glm::vec3(w, h, d);
   }

   tinyxml2::XMLElement* pAngularFactor = pData->FirstChildElement("AngularFactor");
   m_angularFactor = glm::vec3(1.0f, 1.0f, 1.0f);
   if (pAngularFactor) {
      double x = 0;
      double y = 0;
      double z = 0;
      pAngularFactor->QueryDoubleAttribute("x", &x);
      pAngularFactor->QueryDoubleAttribute("y", &y);
      pAngularFactor->QueryDoubleAttribute("z", &z);
      m_angularFactor = glm::vec3(x, y, z);
   }

   return true;
}

void Physics3DComponent::Activate()
{
   std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();

   switch (m_shape) {
      case Shape3D::SPHERE:
         m_gamePhysics->AddSphere((float)m_rigidBodyScale.x, m_bodyType, m_pOwner->GetId(), pTransformComponent->GetPosition(), pTransformComponent->GetRotation(), m_angularFactor, m_density, m_material);
         break;
      case Shape3D::BOX:
         m_gamePhysics->AddBox(m_rigidBodyScale, m_bodyType, m_pOwner->GetId(), pTransformComponent->GetPosition(), pTransformComponent->GetRotation(), m_angularFactor, m_density, m_material);
         break;
      case Shape3D::CAPSULE:
         m_gamePhysics->AddCapsule(m_rigidBodyScale.x, m_rigidBodyScale.y, m_bodyType, m_pOwner->GetId(), pTransformComponent->GetPosition(), pTransformComponent->GetRotation(), m_angularFactor, m_density, m_material);
         break;
      case Shape3D::POINT_CLOUD:
         Logger::WriteLog(Logger::LogType::ERROR, "Not supported yet!");
         break;
      default:
         break;
   }
}

void Physics3DComponent::Deactivate()
{
   m_gamePhysics->RemoveActor(m_pOwner->GetId());
}

tinyxml2::XMLElement* Physics3DComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   // Shape
   tinyxml2::XMLElement* pShape = pDoc->NewElement("Shape");
   tinyxml2::XMLText* pShapeText = nullptr;
   switch (m_shape) {
      case Shape3D::SPHERE:
         pShapeText = pDoc->NewText("Sphere");
         break;
      case Shape3D::BOX:
         pShapeText = pDoc->NewText("Box");
         break;
      case Shape3D::POINT_CLOUD:
         pShapeText = pDoc->NewText("PointCloud");
         break;
      default:
         Logger::WriteLog(Logger::LogType::ERROR, "Unrecognized phycics shape type in Physics3DComponent for actor with ActorID: " + std::to_string(m_pOwner->GetId()));
   }
   pShape->LinkEndChild(pShapeText);
   pBaseElement->LinkEndChild(pShape);

   // BodyType
   tinyxml2::XMLElement* pBodyType = pDoc->NewElement("BodyType");
   tinyxml2::XMLText* pBodyTypeText = nullptr;
   switch (m_bodyType) {
      case IGamePhysics3D::BodyType::DYNAMIC:
         pBodyTypeText = pDoc->NewText("Dynamic");
         break;
      case IGamePhysics3D::BodyType::KINEMATIC:
         pBodyTypeText = pDoc->NewText("Kinematic");
         break;
      case IGamePhysics3D::BodyType::STATIC:
         pBodyTypeText = pDoc->NewText("Static");
         break;
      default:
         Logger::WriteLog(Logger::LogType::ERROR, "Unrecognized phycics body shape type in Physics3DComponent for actor with ActorID: " + std::to_string(m_pOwner->GetId()));
   }
   pBodyType->LinkEndChild(pBodyTypeText);
   pBaseElement->LinkEndChild(pBodyType);

   // Density
   tinyxml2::XMLElement* pDensity = pDoc->NewElement("Density");
   tinyxml2::XMLText* pDensityText = pDoc->NewText(m_density.c_str());
   pDensity->LinkEndChild(pDensityText);
   pBaseElement->LinkEndChild(pDensity);

   // PhysicsMaterial
   tinyxml2::XMLElement* pPhysicsMaterial = pDoc->NewElement("PhysicsMaterial");
   tinyxml2::XMLText* pPhysicsMaterialText = pDoc->NewText(m_material.c_str());
   pPhysicsMaterial->LinkEndChild(pPhysicsMaterialText);
   pBaseElement->LinkEndChild(pPhysicsMaterial);

   tinyxml2::XMLElement* pScale = pDoc->NewElement("Scale");
   pScale->SetAttribute("w", std::to_string(m_rigidBodyScale.x).c_str());
   pScale->SetAttribute("h", std::to_string(m_rigidBodyScale.y).c_str());
   pScale->SetAttribute("d", std::to_string(m_rigidBodyScale.z).c_str());
   pBaseElement->LinkEndChild(pScale);

   tinyxml2::XMLElement* pAngularFactorElement = pDoc->NewElement("AngularFactor");
   pAngularFactorElement->SetAttribute("x", std::to_string(m_angularFactor.x).c_str());
   pAngularFactorElement->SetAttribute("y", std::to_string(m_angularFactor.y).c_str());
   pAngularFactorElement->SetAttribute("z", std::to_string(m_angularFactor.z).c_str());
   pBaseElement->LinkEndChild(pAngularFactorElement);

   return pBaseElement;
}

void Physics3DComponent::ApplyForce(const glm::vec3& direction)
{
   m_gamePhysics->ApplyForce(direction, m_pOwner->GetId());
}

void Physics3DComponent::ApplyTorque(const glm::vec3& torque)
{
   m_gamePhysics->ApplyTorque(torque, m_pOwner->GetId());
}

bool Physics3DComponent::KinematicMove(const glm::vec3& position, const glm::vec3& rotation)
{
   return m_gamePhysics->KinematicMove(m_pOwner->GetId(), position, rotation);
}

glm::vec3 Physics3DComponent::GetVelocity() const
{
   return m_gamePhysics->GetVelocity(m_pOwner->GetId());
}

void Physics3DComponent::SetVelocity(const glm::vec3& velocity)
{
   m_gamePhysics->SetVelocity(m_pOwner->GetId(), velocity);
}

void Physics3DComponent::SetAngularVelocity(const glm::vec3& angles)
{
   m_gamePhysics->SetAngularVelocity(m_pOwner->GetId(), angles);
}

glm::vec3 Physics3DComponent::GetAngularVelocity() const
{
   return m_gamePhysics->GetAngularVelocity(m_pOwner->GetId());
}

void Physics3DComponent::Stop()
{
   return m_gamePhysics->StopActor(m_pOwner->GetId());
}

} // namespace BIEngine
