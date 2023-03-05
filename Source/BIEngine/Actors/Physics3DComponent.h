#pragma once

#include "ActorComponent.h"
#include "../Physics/Physics3D.h"

namespace BIEngine
{

    class Physics3DComponent : public ActorComponent
    {
    public:
        static const ComponentId g_CompId;
        virtual ComponentId GetComponentId() const { return Physics3DComponent::g_CompId; };

    public:
        Physics3DComponent();
        virtual ~Physics3DComponent();

        virtual bool Init(tinyxml2::XMLElement* pData) override;
        virtual void PostInit() override;

        virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

        void ApplyForce(const glm::vec3& direction);
        void ApplyTorque(const glm::vec3& torque);
        bool KinematicMove(const glm::vec3& position, const glm::vec3& rotation);

        void SetVelocity(const glm::vec3& velocity);
        glm::vec3 GetVelocity() const;
        void SetAngularVelocity(const glm::vec3& angles);
        glm::vec3 GetAngularVelocity() const;
        void Stop();

    protected:
        enum class Shape3D
        {
            BOX,
            SPHERE,
            POINT_CLOUD,
            NONE
        };


        Shape3D m_shape;
        IGamePhysics3D::BodyType m_bodyType;
        std::string m_density;
        std::string m_material;

        glm::vec3 m_rigidBodyScale;

        std::shared_ptr<IGamePhysics3D> m_gamePhysics;
    };

    static ActorComponent* CreatePhysics3DComponent()
    {
        return new Physics3DComponent;
    }
}


