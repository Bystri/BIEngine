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
      : m_position(0.0, 0.0, 0.0), m_size(1.0, 1.0, 1.0), m_rotation(0), m_worldTransform(1.0f), m_localTransform(1.0f) {}

   virtual bool Init(tinyxml2::XMLElement* pData);

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   virtual void OnUpdate(const GameTimer& gt) override;

   // Задание и получение свойств
   glm::vec3 GetPosition() const { return m_position; }

   void SetPosition(const glm::vec3& pos);

   glm::vec3 GetRotation() const { return m_rotation; }

   glm::vec3 GetDir() const;

   void SetRotation(const glm::vec3& rot);

   glm::vec3 GetSize() const { return m_size; }

   void SetSize(const glm::vec3& size);

   void SetLocalTransformMatrix(const glm::mat4& trans);

   glm::mat4 GetWorldTransformMatrix() const { return m_worldTransform; }

   glm::mat4 GetLocalTransformMatrix() const { return m_localTransform; }

private:
   void recalculateLocalTransformMatrix();
   void updateWorldTransformMatrix();

private:
   glm::mat4 m_worldTransform;
   glm::mat4 m_localTransform;

   glm::vec3 m_position;
   glm::vec3 m_size;
   glm::vec3 m_rotation;
};

static std::unique_ptr<ActorComponent> CreateTransformComponent()
{
   return std::make_unique<TransformComponent>();
}

} // namespace BIEngine
