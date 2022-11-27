
#include "PhysicsTriggerComponent.h"
#include "TransformComponent.h"
#include "../Physics/Physics.h"
#include "../Utilities/String.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine
{

    const ComponentId PhysicsTriggerComponent::g_CompId = "PhysicsTriggerComponent";

    PhysicsTriggerComponent::PhysicsTriggerComponent()
    {
        m_dimension = glm::vec3(1.f, 1.f, 1.f);
    }

    PhysicsTriggerComponent::~PhysicsTriggerComponent()
    {
        m_gamePhysics->RemoveActor(m_pOwner->GetId());
    }

    bool PhysicsTriggerComponent::Init(tinyxml2::XMLElement* pData)
    {
        m_gamePhysics = g_pApp->m_pGameLogic->GetGamePhysics();
        if (!m_gamePhysics)
            return false; //Если нет физического движка, то и смысла в дальнейшем использовании компонента тоже нет


        tinyxml2::XMLElement* pScaleElement = pData->FirstChildElement("Scale");
        if (pScaleElement)
        {
            double w = 0;
            double h = 0;
            pScaleElement->QueryDoubleAttribute("w", &w);
            pScaleElement->QueryDoubleAttribute("h", &h);
            m_dimension = glm::vec3(w, h, 1);
        }

        return true;
    }

    void PhysicsTriggerComponent::PostInit()
    {
        m_gamePhysics->CreateTrigger(m_pOwner, m_dimension);
    }

    tinyxml2::XMLElement* PhysicsTriggerComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
    {
        tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

        tinyxml2::XMLElement* pScale = pDoc->NewElement("Scale");
        pScale->SetAttribute("w", std::to_string(m_dimension.x).c_str());
        pScale->SetAttribute("h", std::to_string(m_dimension.y).c_str());
        pBaseElement->LinkEndChild(pScale);

        return pBaseElement;
    }

    glm::vec3 PhysicsTriggerComponent::GetVelocity()
    {
        return m_gamePhysics->GetVelocity(m_pOwner->GetId());
    }

    void PhysicsTriggerComponent::SetVelocity(const glm::vec3& velocity)
    {
        m_gamePhysics->SetVelocity(m_pOwner->GetId(), velocity);
    }

    void PhysicsTriggerComponent::Rotate(float angleRadians)
    {
        m_gamePhysics->Rotate(m_pOwner->GetId(), angleRadians);
    }

    void PhysicsTriggerComponent::SetPosition(float x, float y, float z)
    {
        m_gamePhysics->SetPosition(m_pOwner->GetId(), glm::vec3(x, y, z));
    }

    void PhysicsTriggerComponent::Stop()
    {
        return m_gamePhysics->StopActor(m_pOwner->GetId());
    }

}
