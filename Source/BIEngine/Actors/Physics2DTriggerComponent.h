#pragma once

#include "ActorComponent.h"
#include "../Physics/Physics2D.h"

namespace BIEngine {

// Прикрепляет к актеру триггерную зону в виде куба
class Physics2DTriggerComponent : public ActorComponent {
public:
   static const ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return Physics2DTriggerComponent::g_CompId; };

public:
   Physics2DTriggerComponent();
   virtual ~Physics2DTriggerComponent();

   virtual bool Init(tinyxml2::XMLElement* pData) override;
   virtual void PostInit() override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   glm::vec2 GetVelocity();
   void SetVelocity(const glm::vec2& velocity);
   void Rotate(float angle);
   void SetPosition(const glm::vec2& position);
   void Stop();


protected:
   void BuildRigidBodyTransform(tinyxml2::XMLElement* pTransformElement);

protected:
   glm::vec2 m_dimension;

   std::shared_ptr<IGamePhysics2D> m_gamePhysics;
};

static ActorComponent* CreatePhysics2DTriggerComponent()
{
   return new Physics2DTriggerComponent;
}
} // namespace BIEngine
