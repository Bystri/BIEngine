#pragma once

#include "ActorComponent.h"
#include "../Physics/Physics3D.h"

namespace BIEngine {
// Прикрепляет к актеру триггерную зону в виде куба
class Physics3DTriggerComponent : public ActorComponent {
public:
   static const ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return Physics3DTriggerComponent::g_CompId; };

public:
   Physics3DTriggerComponent();
   virtual ~Physics3DTriggerComponent();

   virtual bool Init(tinyxml2::XMLElement* pData) override;
   virtual void Activate() override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   glm::vec3 GetVelocity();
   void SetVelocity(const glm::vec3& velocity);
   void RotateY(float angle);
   void SetPosition(const glm::vec3& position);
   void Stop();


protected:
   void BuildRigidBodyTransform(tinyxml2::XMLElement* pTransformElement);

protected:
   glm::vec3 m_dimension;

   std::shared_ptr<IGamePhysics3D> m_gamePhysics;
};

static ActorComponent* CreatePhysics3DTriggerComponent()
{
   return new Physics3DTriggerComponent;
}
} // namespace BIEngine
