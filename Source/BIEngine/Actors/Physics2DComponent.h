#pragma once

#include "ActorComponent.h"
#include "../StdLib/UniquePtr.h"
#include "../Physics/Physics2D.h"

namespace BIEngine {

class Physics2DComponent : public ActorComponent {
public:
   static const ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return Physics2DComponent::g_CompId; };

public:
   Physics2DComponent();

   virtual bool Init(tinyxml2::XMLElement* pData) override;
   virtual void Activate() override;
   virtual void Deactivate() override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   void ApplyForce(const glm::vec2& direction);
   void ApplyTorque(float torque);
   bool KinematicMove(const glm::vec2& position, float rotation);

   void SetVelocity(const glm::vec2& velocity);
   glm::vec2 GetVelocity() const;
   void SetAngularVelocity(float angle);
   float GetAngularVelocity() const;
   void Stop();

protected:
   enum class Shape2D {
      BOX,
      CIRCLE,
      POINT_CLOUD,
      NONE
   };


   Shape2D m_shape;
   IGamePhysics2D::BodyType m_bodyType;
   String m_density;
   String m_material;

   glm::vec2 m_rigidBodyScale;

   SharedPtr<IGamePhysics2D> m_gamePhysics;
};

static UniquePtr<ActorComponent> CreatePhysics2DComponent()
{
   return MakeUnique<Physics2DComponent>();
}
} // namespace BIEngine
