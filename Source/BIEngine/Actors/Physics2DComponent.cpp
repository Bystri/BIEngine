#include "Physics2DComponent.h"

#include "TransformComponent.h"
#include "../Physics/Physics3D.h"
#include "../Utilities/String.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine {

const ComponentId Physics2DComponent::g_CompId = "Physics2DComponent";

Physics2DComponent::Physics2DComponent()
{
   m_rigidBodyScale = glm::vec2(1.f, 1.f);

   m_shape = Shape2D::NONE;
   m_bodyType = IGamePhysics2D::BodyType::DYNAMIC;
}

Physics2DComponent::~Physics2DComponent()
{
   m_gamePhysics->RemoveActor(m_pOwner->GetId());
}

bool Physics2DComponent::Init(tinyxml2::XMLElement* pData)
{
   m_gamePhysics = g_pApp->m_pGameLogic->GetGamePhysics2D();
   if (!m_gamePhysics)
      return false; // Если нет физического движка, то и смысла в дальнейшем использовании компонента тоже нет

   tinyxml2::XMLElement* pShape = pData->FirstChildElement("Shape");
   if (pShape) {
      std::string shapeStr = pShape->FirstChild()->Value();
      if (shapeStr == "Circle")
         m_shape = Shape2D::CIRCLE;
      else if (shapeStr == "Box")
         m_shape = Shape2D::BOX;
      else if (shapeStr == "PointCloud")
         m_shape = Shape2D::POINT_CLOUD;
   }

   tinyxml2::XMLElement* pBodyType = pData->FirstChildElement("BodyType");
   if (pBodyType) {
      std::string typeStr = pBodyType->FirstChild()->Value();
      if (typeStr == "Dynamic")
         m_bodyType = IGamePhysics2D::BodyType::DYNAMIC;
      else if (typeStr == "Kinematic")
         m_bodyType = IGamePhysics2D::BodyType::KINEMATIC;
      else if (typeStr == "Static")
         m_bodyType = IGamePhysics2D::BodyType::STATIC;
   }

   tinyxml2::XMLElement* pDensity = pData->FirstChildElement("Density");
   if (pDensity)
      m_density = pDensity->FirstChild()->Value();

   tinyxml2::XMLElement* pMaterial = pData->FirstChildElement("PhysicsMaterial");
   if (pMaterial)
      m_material = pMaterial->FirstChild()->Value();

   tinyxml2::XMLElement* pSizeElement = pData->FirstChildElement("Scale");
   if (pSizeElement) {
      double w = 0;
      double h = 0;
      pSizeElement->QueryDoubleAttribute("w", &w);
      pSizeElement->QueryDoubleAttribute("h", &h);
      m_rigidBodyScale = glm::vec3(w, h, 1);
   }

   return true;
}

void Physics2DComponent::PostInit()
{
   if (m_pOwner) {
      switch (m_shape) {
         case Shape2D::CIRCLE:
            m_gamePhysics->AddCircle((float)m_rigidBodyScale.x, m_bodyType, m_pOwner, m_density, m_material);
            break;
         case Shape2D::BOX:
            m_gamePhysics->AddBox(m_rigidBodyScale, m_bodyType, m_pOwner, m_density, m_material);
            break;
         case Shape2D::POINT_CLOUD:
            Logger::WriteLog(Logger::LogType::ERROR, "Not supported yet!");
            break;
         default:
            break;
      }
   }
}

tinyxml2::XMLElement* Physics2DComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   // Shape
   tinyxml2::XMLElement* pShape = pDoc->NewElement("Shape");
   tinyxml2::XMLText* pShapeText = nullptr;
   switch (m_shape) {
      case Shape2D::CIRCLE:
         pShapeText = pDoc->NewText("Circle");
         break;
      case Shape2D::BOX:
         pShapeText = pDoc->NewText("Box");
         break;
      case Shape2D::POINT_CLOUD:
         pShapeText = pDoc->NewText("PointCloud");
         break;
      default:
         Logger::WriteLog(Logger::LogType::ERROR, "Unrecognized phycics shape type in Physics2DComponent for actor with ActorID: " + std::to_string(m_pOwner->GetId()));
   }
   pShape->LinkEndChild(pShapeText);
   pBaseElement->LinkEndChild(pShape);

   // BodyType
   tinyxml2::XMLElement* pBodyType = pDoc->NewElement("BodyType");
   tinyxml2::XMLText* pBodyTypeText = nullptr;
   switch (m_bodyType) {
      case IGamePhysics2D::BodyType::DYNAMIC:
         pBodyTypeText = pDoc->NewText("Dynamic");
         break;
      case IGamePhysics2D::BodyType::KINEMATIC:
         pBodyTypeText = pDoc->NewText("Kinematic");
         break;
      case IGamePhysics2D::BodyType::STATIC:
         pBodyTypeText = pDoc->NewText("Static");
         break;
      default:
         Logger::WriteLog(Logger::LogType::ERROR, "Unrecognized phycics body shape type in Physics2DComponent for actor with ActorID: " + std::to_string(m_pOwner->GetId()));
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
   pBaseElement->LinkEndChild(pScale);

   return pBaseElement;
}

void Physics2DComponent::ApplyForce(const glm::vec2& direction)
{
   m_gamePhysics->ApplyForce(direction, m_pOwner->GetId());
}

void Physics2DComponent::ApplyTorque(float torque)
{
   m_gamePhysics->ApplyTorque(torque, m_pOwner->GetId());
}

bool Physics2DComponent::KinematicMove(const glm::vec2& position, float rotation)
{
   return m_gamePhysics->KinematicMove(m_pOwner->GetId(), position, rotation);
}

glm::vec2 Physics2DComponent::GetVelocity() const
{
   return m_gamePhysics->GetVelocity(m_pOwner->GetId());
}

void Physics2DComponent::SetVelocity(const glm::vec2& velocity)
{
   m_gamePhysics->SetVelocity(m_pOwner->GetId(), velocity);
}

void Physics2DComponent::SetAngularVelocity(float angle)
{
   m_gamePhysics->SetAngularVelocity(m_pOwner->GetId(), angle);
}

float Physics2DComponent::GetAngularVelocity() const
{
   return m_gamePhysics->GetAngularVelocity(m_pOwner->GetId());
}

void Physics2DComponent::Stop()
{
   return m_gamePhysics->StopActor(m_pOwner->GetId());
}

} // namespace BIEngine
