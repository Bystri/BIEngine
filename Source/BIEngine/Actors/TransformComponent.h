#pragma once

#include "ActorComponent.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace BIEngine {

// Компонент, отвечающий за положение и размер актера
class TransformComponent : public ActorComponent {
public:
   static const ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return TransformComponent::g_CompId; };

   TransformComponent()
      : m_position(0.0, 0.0, 0.0), m_size(1.0, 1.0, 1.0), m_rotation(0), m_transform(1.0f) {}

   virtual bool Init(tinyxml2::XMLElement* pData);

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   // Задание и получение свойств
   glm::vec3 GetPosition() const { return m_position; }

   void SetPosition(const glm::vec3& pos);

   glm::vec3 GetRotation() const { return m_rotation; }

   void SetRotation(const glm::vec3& rot);

   glm::vec3 GetSize() const { return m_size; }

   void SetSize(const glm::vec3& size);

   void SetTransformMatrix(const glm::mat4& trans);

   glm::mat4 GetTransformMatrix() const { return m_transform; }

private:
   void RecalculateTransformMatrix();

private:
   glm::mat4 m_transform;

   glm::vec3 m_position;
   glm::vec3 m_size;
   glm::vec3 m_rotation;
};

static ActorComponent* CreateTransformComponent()
{
   return new TransformComponent;
}

} // namespace BIEngine
