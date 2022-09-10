#pragma once

#include "ActorComponent.h"
#include "../Physics/Physics.h"

namespace BIEngine
{

    class PhysicsComponent : public ActorComponent
    {
    public:
        static const ComponentId g_CompId;
        virtual ComponentId GetComponentId() const { return PhysicsComponent::g_CompId; };

    public:
        PhysicsComponent();
        virtual ~PhysicsComponent();

        virtual bool Init(tinyxml2::XMLElement* pData) override;
        virtual void PostInit() override;

        virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

        void ApplyForce(const glm::vec2& direction, float forceNewtons);
        void ApplyTorque(const glm::vec2& direction, float forceNewtons);
        bool KinematicMove(const glm::vec2& position, float rotate);

        glm::vec2 GetVelocity();
        void SetVelocity(const glm::vec2& velocity);
        void Rotate(float angle);
        void SetPosition(float x, float y);
        void Stop();

    protected:
        enum class Shape
        {
            BOX,
            CIRCLE,
            POINT_CLOUD,
            NONE
        };


        Shape m_shape;
        IGamePhysics2D::BodyType m_bodyType;
        std::string m_density;
        std::string m_material;

        glm::vec2 m_rigidBodyScale;

        std::shared_ptr<IGamePhysics2D> m_gamePhysics;
    };

    static ActorComponent* CreatePhysicsComponent()
    {
        return new PhysicsComponent;
    }
}


