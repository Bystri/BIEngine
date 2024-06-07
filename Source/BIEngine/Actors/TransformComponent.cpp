#include "TransformComponent.h"

#include <string>

#include <glm/gtx/matrix_decompose.hpp>

#include "../EngineCore/Assert.h"
#include "../Actors/Actor.h"

namespace BIEngine {

const ComponentId TransformComponent::g_CompId = "TransformComponent";

bool TransformComponent::Init(tinyxml2::XMLElement* pData)
{
   Assert(pData, "Bad arguments provided for initialization to Transorm Component");

   tinyxml2::XMLElement* pPositionElement = pData->FirstChildElement("Position");
   if (pPositionElement) {
      double x = 0;
      double y = 0;
      double z = 0;
      pPositionElement->QueryDoubleAttribute("x", &x);
      pPositionElement->QueryDoubleAttribute("y", &y);
      pPositionElement->QueryDoubleAttribute("z", &z);
      SetLocalPosition(glm::vec3(x, y, z));
   }

   tinyxml2::XMLElement* pSizeElement = pData->FirstChildElement("Size");
   if (pSizeElement) {
      double w = 0;
      double h = 0;
      double d = 0;
      pSizeElement->QueryDoubleAttribute("w", &w);
      pSizeElement->QueryDoubleAttribute("h", &h);
      pSizeElement->QueryDoubleAttribute("d", &d);
      SetLocalSize(glm::vec3(w, h, d));
   }

   tinyxml2::XMLElement* pRotationElement = pData->FirstChildElement("Rotation");
   if (pRotationElement) {
      double x = 0;
      double y = 0;
      double z = 0;
      pRotationElement->QueryDoubleAttribute("x", &x);
      pRotationElement->QueryDoubleAttribute("y", &y);
      pRotationElement->QueryDoubleAttribute("z", &z);
      SetLocalRotation(glm::vec3(x, y, z));
   }

   recalculateLocalTransformMatrix();
   updateWorldTransformMatrix();

   return true;
}

tinyxml2::XMLElement* TransformComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   tinyxml2::XMLElement* pPosition = pDoc->NewElement("Position");
   pPosition->SetAttribute("x", std::to_string(m_localPos.x).c_str());
   pPosition->SetAttribute("y", std::to_string(m_localPos.y).c_str());
   pPosition->SetAttribute("z", std::to_string(m_localPos.z).c_str());
   pBaseElement->LinkEndChild(pPosition);

   tinyxml2::XMLElement* pSize = pDoc->NewElement("Size");
   pSize->SetAttribute("w", std::to_string(m_localSize.x).c_str());
   pSize->SetAttribute("h", std::to_string(m_localSize.y).c_str());
   pSize->SetAttribute("d", std::to_string(m_localSize.z).c_str());
   pBaseElement->LinkEndChild(pSize);

   tinyxml2::XMLElement* pRotation = pDoc->NewElement("Rotation");
   pRotation->SetAttribute("x", std::to_string(m_localRot.x).c_str());
   pRotation->SetAttribute("y", std::to_string(m_localRot.y).c_str());
   pRotation->SetAttribute("z", std::to_string(m_localRot.z).c_str());
   pBaseElement->LinkEndChild(pRotation);

   return pBaseElement;
}

void TransformComponent::OnUpdate(const GameTimer& gt)
{
   updateWorldTransformMatrix();
}

void TransformComponent::SetLocalPosition(const glm::vec3& pos)
{
   m_localPos = pos;

   if (m_pOwner->GetParent()) {
      std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetParent()->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();

      if (pTransformComponent != nullptr) {
         m_worldPos = pTransformComponent->GetLocalToWorldTransformMatrix() * glm::vec4(m_localPos, 1.0f);
         recalculateWorldTransformMatrix();
         recalculateLocalTransformMatrix();
         return;
      }
   }

   m_worldPos = m_localPos;
   recalculateWorldTransformMatrix();
   recalculateLocalTransformMatrix();
}

void TransformComponent::SetLocalRotation(const glm::vec3& rot)
{
   m_localRot = rot;

   if (m_pOwner->GetParent()) {
      std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetParent()->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();

      if (pTransformComponent != nullptr) {
         m_worldRot = pTransformComponent->GetLocalToWorldTransformMatrix() * glm::vec4(m_localRot, 1.0f);
         recalculateWorldTransformMatrix();
         recalculateLocalTransformMatrix();
         return;
      }
   }

   m_worldRot = m_localRot;
   recalculateWorldTransformMatrix();
   recalculateLocalTransformMatrix();
}

void TransformComponent::SetLocalSize(const glm::vec3& size)
{
   m_localSize = size;

   if (m_pOwner->GetParent()) {
      std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetParent()->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();

      if (pTransformComponent != nullptr) {
         m_worldSize = pTransformComponent->GetLocalToWorldTransformMatrix() * glm::vec4(m_localSize, 1.0f);
         recalculateWorldTransformMatrix();
         recalculateLocalTransformMatrix();
         return;
      }
   }

   m_worldSize = m_localSize;
   recalculateWorldTransformMatrix();
   recalculateLocalTransformMatrix();
}

void TransformComponent::SetLocalTransformMatrix(const glm::mat4& trans)
{
   glm::quat qRotation;
   glm::vec3 translation;
   glm::vec3 skew;
   glm::vec4 perspective;
   glm::decompose(trans, m_localSize, qRotation, m_localPos, skew, perspective);

   m_localRot = glm::eulerAngles(qRotation);
   m_localRot.x = glm::degrees(m_localRot.x);
   m_localRot.y = glm::degrees(m_localRot.y);
   m_localRot.z = glm::degrees(m_localRot.z);

   m_localTransform = trans;
}

glm::vec3 TransformComponent::GetDir() const
{
   // Calculate forward vector
   glm::vec4 dir = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

   const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(m_localRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
   const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(m_localRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
   const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_localRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

   const glm::mat4 roationMatrix = transformZ * transformY * transformX;

   dir = roationMatrix * dir;

   return glm::vec3(dir);
}

void TransformComponent::SetPosition(const glm::vec3& pos)
{
   m_worldPos = pos;

   if (m_pOwner->GetParent()) {
      std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetParent()->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();

      if (pTransformComponent != nullptr) {
         m_localPos = pTransformComponent->GetWorldToLocalTransformMatrix() * glm::vec4(m_worldPos, 1.0f);
         recalculateLocalTransformMatrix();
         recalculateLocalTransformMatrix();
         return;
      }
   }

   m_localPos = m_worldPos;
   recalculateLocalTransformMatrix();
   recalculateLocalTransformMatrix();
}

void TransformComponent::SetRotation(const glm::vec3& rot)
{
   m_worldRot = rot;

   if (m_pOwner->GetParent()) {
      std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetParent()->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();

      if (pTransformComponent != nullptr) {
         m_localRot = pTransformComponent->GetWorldToLocalTransformMatrix() * glm::vec4(m_worldRot, 1.0f);
         recalculateLocalTransformMatrix();
         recalculateLocalTransformMatrix();
         return;
      }
   }

   m_localRot = m_worldRot;
   recalculateLocalTransformMatrix();
   recalculateLocalTransformMatrix();
}

void TransformComponent::SetSize(const glm::vec3& size)
{
   m_worldSize = size;

   if (m_pOwner->GetParent()) {
      std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetParent()->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();

      if (pTransformComponent != nullptr) {
         m_localSize = pTransformComponent->GetWorldToLocalTransformMatrix() * glm::vec4(m_worldSize, 1.0f);
         recalculateLocalTransformMatrix();
         recalculateLocalTransformMatrix();
         return;
      }
   }

   m_localSize = m_worldSize;
   recalculateLocalTransformMatrix();
   recalculateLocalTransformMatrix();
}

void TransformComponent::recalculateLocalTransformMatrix()
{
   const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(m_localRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
   const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(m_localRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
   const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_localRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

   const glm::mat4 roationMatrix = transformZ * transformY * transformX;

   // translation * rotation * scale
   m_localTransform = glm::translate(glm::mat4(1.0f), m_localPos) *
                      roationMatrix *
                      glm::scale(glm::mat4(1.0f), m_localSize);

   m_localToWorld = glm::inverse(m_localTransform);
}

void TransformComponent::recalculateWorldTransformMatrix()
{
   const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(m_worldRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
   const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(m_worldRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
   const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_worldRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

   const glm::mat4 roationMatrix = transformZ * transformY * transformX;

   // translation * rotation * scale
   m_worldTransform = glm::translate(glm::mat4(1.0f), m_worldPos) *
                      roationMatrix *
                      glm::scale(glm::mat4(1.0f), m_worldSize);

   m_worldToLocal = glm::inverse(m_worldTransform);
}

void TransformComponent::updateWorldTransformMatrix()
{
   if (m_pOwner->GetParent()) {
      std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetParent()->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();

      if (pTransformComponent != nullptr) {
         m_worldTransform = pTransformComponent->GetWorldTransformMatrix() * m_localTransform;
      } else {
         m_worldTransform = m_localTransform;
      }
   } else {
      m_worldTransform = m_localTransform;
   }

   applyWorldTransformMatrix(m_worldTransform);
}

void TransformComponent::applyWorldTransformMatrix(const glm::mat4& trans)
{
   glm::quat qRotation;
   glm::vec3 translation;
   glm::vec3 skew;
   glm::vec4 perspective;
   glm::decompose(trans, m_worldSize, qRotation, m_worldPos, skew, perspective);

   m_worldRot = glm::eulerAngles(qRotation);
   m_worldRot.x = glm::degrees(m_worldRot.x);
   m_worldRot.y = glm::degrees(m_worldRot.y);
   m_worldRot.z = glm::degrees(m_worldRot.z);

   m_worldToLocal = glm::inverse(trans);
}

} // namespace BIEngine
