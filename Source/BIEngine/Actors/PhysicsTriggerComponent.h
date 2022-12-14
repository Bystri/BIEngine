#pragma once

#include "ActorComponent.h"
#include "../Physics/Physics.h"

namespace BIEngine
{
   //Прикрепляет к актеру триггерную зону в виде куба
    class PhysicsTriggerComponent : public ActorComponent
    {
    public:
        static const ComponentId g_CompId;
        virtual ComponentId GetComponentId() const { return PhysicsTriggerComponent::g_CompId; };

    public:
        PhysicsTriggerComponent();
        virtual ~PhysicsTriggerComponent();

        virtual bool Init(tinyxml2::XMLElement* pData) override;
        virtual void PostInit() override;

        virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

        glm::vec3 GetVelocity();
        void SetVelocity(const glm::vec3& velocity);
        void Rotate(float angle);
        void SetPosition(float x, float y, float z);
        void Stop();


    protected:
        void BuildRigidBodyTransform(tinyxml2::XMLElement* pTransformElement);

    protected:
        glm::vec3 m_dimension;

        std::shared_ptr<IGamePhysics> m_gamePhysics;
    };

    static ActorComponent* CreatePhysicsTriggerComponent()
    {
        return new PhysicsTriggerComponent;
    }
}


