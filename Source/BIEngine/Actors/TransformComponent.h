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
      : m_localPos(0.0f), m_localRot(0.0f), m_localSize(1.0f),
        m_worldPos(0.0f), m_worldRot(0.0f), m_worldSize(1.0f),
        m_worldTransform(1.0f), m_localTransform(1.0f),
        m_localToWorld(1.0f), m_worldToLocal(1.0f) {}

   virtual bool Init(tinyxml2::XMLElement* pData);

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   virtual void OnUpdate(const GameTimer& gt) override;

   // Задание и получение свойств
   glm::vec3 GetLocalPosition() const { return m_localPos; }

   void SetLocalPosition(const glm::vec3& pos);

   glm::vec3 GetLocalRotation() const { return m_localRot; }

   glm::vec3 GetDir() const;

   void SetLocalRotation(const glm::vec3& rot);

   glm::vec3 GetLocalSize() const { return m_localSize; }

   void SetLocalSize(const glm::vec3& size);

   glm::vec3 GetPosition() const { return m_worldPos; }

   void SetPosition(const glm::vec3& pos);

   glm::vec3 GetRotation() const { return m_worldRot; }

   void SetRotation(const glm::vec3& rot);

   glm::vec3 GetSize() const { return m_worldSize; }

   void SetSize(const glm::vec3& size);

   void SetLocalTransformMatrix(const glm::mat4& trans);

   glm::mat4 GetWorldTransformMatrix() const { return m_worldTransform; }

   glm::mat4 GetWorldToLocalTransformMatrix() const { return m_worldToLocal; }

   glm::mat4 GetLocalTransformMatrix() const { return m_localTransform; }

   glm::mat4 GetLocalToWorldTransformMatrix() const { return m_localToWorld; }

private:
   void recalculateLocalTransformMatrix();
   void recalculateWorldTransformMatrix();
   void updateWorldTransformMatrix();
   void applyWorldTransformMatrix(const glm::mat4& trans);

private:
   glm::mat4 m_worldTransform;
   glm::mat4 m_localTransform;
   glm::mat4 m_worldToLocal;
   glm::mat4 m_localToWorld;

   glm::vec3 m_worldPos;
   glm::vec3 m_worldRot;
   glm::vec3 m_worldSize;

   glm::vec3 m_localPos;
   glm::vec3 m_localRot;
   glm::vec3 m_localSize;
};

static std::unique_ptr<ActorComponent> CreateTransformComponent()
{
   return std::make_unique<TransformComponent>();
}

} // namespace BIEngine
