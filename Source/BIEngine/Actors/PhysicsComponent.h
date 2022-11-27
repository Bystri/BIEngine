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

        void ApplyForce(const glm::vec3& direction);
        void ApplyTorque(float torque);
        bool KinematicMove(const glm::vec3& position, float rotate);

        void SetVelocity(const glm::vec3& velocity);
        glm::vec3 GetVelocity() const;
        void Rotate(float angle);
        float GetRotation() const;
        void SetPosition(float x, float y, float z);
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
        IGamePhysics::BodyType m_bodyType;
        std::string m_density;
        std::string m_material;

        glm::vec3 m_rigidBodyScale;

        std::shared_ptr<IGamePhysics> m_gamePhysics;
    };

    static ActorComponent* CreatePhysicsComponent()
    {
        return new PhysicsComponent;
    }
}


