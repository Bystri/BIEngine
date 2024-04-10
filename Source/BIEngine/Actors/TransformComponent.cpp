#include "TransformComponent.h"

#include <string>

#include <glm/gtx/matrix_decompose.hpp>

#include "../EngineCore/Assert.h"

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
      SetPosition(glm::vec3(x, y, z));
   }

   tinyxml2::XMLElement* pSizeElement = pData->FirstChildElement("Size");
   if (pSizeElement) {
      double w = 0;
      double h = 0;
      double d = 0;
      pSizeElement->QueryDoubleAttribute("w", &w);
      pSizeElement->QueryDoubleAttribute("h", &h);
      pSizeElement->QueryDoubleAttribute("d", &d);
      SetSize(glm::vec3(w, h, d));
   }

   tinyxml2::XMLElement* pRotationElement = pData->FirstChildElement("Rotation");
   if (pRotationElement) {
      double x = 0;
      double y = 0;
      double z = 0;
      pRotationElement->QueryDoubleAttribute("x", &x);
      pRotationElement->QueryDoubleAttribute("y", &y);
      pRotationElement->QueryDoubleAttribute("z", &z);
      SetRotation(glm::vec3(x, y, z));
   }

   return true;
}

tinyxml2::XMLElement* TransformComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
{
   tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

   tinyxml2::XMLElement* pPosition = pDoc->NewElement("Position");
   pPosition->SetAttribute("x", std::to_string(m_position.x).c_str());
   pPosition->SetAttribute("y", std::to_string(m_position.y).c_str());
   pPosition->SetAttribute("z", std::to_string(m_position.z).c_str());
   pBaseElement->LinkEndChild(pPosition);

   tinyxml2::XMLElement* pSize = pDoc->NewElement("Size");
   pSize->SetAttribute("w", std::to_string(m_size.x).c_str());
   pSize->SetAttribute("h", std::to_string(m_size.y).c_str());
   pSize->SetAttribute("d", std::to_string(m_size.z).c_str());
   pBaseElement->LinkEndChild(pSize);

   tinyxml2::XMLElement* pRotation = pDoc->NewElement("Rotation");
   pRotation->SetAttribute("x", std::to_string(m_rotation.x).c_str());
   pRotation->SetAttribute("y", std::to_string(m_rotation.y).c_str());
   pRotation->SetAttribute("z", std::to_string(m_rotation.z).c_str());
   pBaseElement->LinkEndChild(pRotation);

   return pBaseElement;
}

void TransformComponent::SetPosition(const glm::vec3& pos)
{
   m_position = pos;
   m_transform[3][0] = m_size.x;
   m_transform[3][1] = m_size.y;
   m_transform[3][2] = m_size.z;
}

void TransformComponent::SetRotation(const glm::vec3& rot)
{
   m_rotation = rot;
   RecalculateTransformMatrix();
}

void TransformComponent::SetSize(const glm::vec3& size)
{
   m_size = size;
   RecalculateTransformMatrix();
}

void TransformComponent::SetTransformMatrix(const glm::mat4& trans)
{
   glm::quat qRotation;
   glm::vec3 translation;
   glm::vec3 skew;
   glm::vec4 perspective;
   glm::decompose(trans, m_size, qRotation, m_position, skew, perspective);

   m_rotation = glm::eulerAngles(qRotation);
   m_rotation.x = glm::degrees(m_rotation.x);
   m_rotation.y = glm::degrees(m_rotation.y);
   m_rotation.z = glm::degrees(m_rotation.z);

   m_transform = trans;
}

glm::vec3 TransformComponent::GetDir() const
{
   // Calculate forward vector
   glm::vec4 dir = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

   const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
   const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
   const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

   const glm::mat4 roationMatrix = transformZ * transformY * transformX;

   dir = roationMatrix * dir;

   return glm::vec3(dir);
}

void TransformComponent::RecalculateTransformMatrix()
{
   const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
   const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
   const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

   const glm::mat4 roationMatrix = transformZ * transformY * transformX;

   // translation * rotation * scale
   m_transform = glm::translate(glm::mat4(1.0f), m_position) *
                 roationMatrix *
                 glm::scale(glm::mat4(1.0f), m_size);
}
} // namespace BIEngine
