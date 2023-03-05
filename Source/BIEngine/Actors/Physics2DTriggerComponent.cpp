
#include "Physics2DTriggerComponent.h"
#include "TransformComponent.h"
#include "../Physics/Physics3D.h"
#include "../Utilities/String.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine
{

    const ComponentId Physics2DTriggerComponent::g_CompId = "Physics2DTriggerComponent";

    Physics2DTriggerComponent::Physics2DTriggerComponent()
    {
        m_dimension = glm::vec2(1.f, 1.f);
    }

    Physics2DTriggerComponent::~Physics2DTriggerComponent()
    {
        m_gamePhysics->RemoveActor(m_pOwner->GetId());
    }

    bool Physics2DTriggerComponent::Init(tinyxml2::XMLElement* pData)
    {
        m_gamePhysics = g_pApp->m_pGameLogic->GetGamePhysics2D();
        if (!m_gamePhysics)
            return false; //Если нет физического движка, то и смысла в дальнейшем использовании компонента тоже нет


        tinyxml2::XMLElement* pScaleElement = pData->FirstChildElement("Scale");
        if (pScaleElement)
        {
            double w = 0;
            double h = 0;
            pScaleElement->QueryDoubleAttribute("w", &w);
            pScaleElement->QueryDoubleAttribute("h", &h);
            m_dimension = glm::vec2(w, h);
        }

        return true;
    }

    void Physics2DTriggerComponent::PostInit()
    {
        m_gamePhysics->CreateTrigger(m_pOwner, m_dimension);
    }

    tinyxml2::XMLElement* Physics2DTriggerComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
    {
        tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

        tinyxml2::XMLElement* pScale = pDoc->NewElement("Scale");
        pScale->SetAttribute("w", std::to_string(m_dimension.x).c_str());
        pScale->SetAttribute("h", std::to_string(m_dimension.y).c_str());
        pBaseElement->LinkEndChild(pScale);

        return pBaseElement;
    }

    glm::vec2 Physics2DTriggerComponent::GetVelocity()
    {
        return m_gamePhysics->GetVelocity(m_pOwner->GetId());
    }

    void Physics2DTriggerComponent::SetVelocity(const glm::vec2& velocity)
    {
        m_gamePhysics->SetVelocity(m_pOwner->GetId(), velocity);
    }

    void Physics2DTriggerComponent::Rotate(float angle)
    {
        m_gamePhysics->Rotate(m_pOwner->GetId(), angle);
    }

    void Physics2DTriggerComponent::SetPosition(const glm::vec2& position)
    {
        m_gamePhysics->SetPosition(m_pOwner->GetId(), position);
    }

    void Physics2DTriggerComponent::Stop()
    {
        return m_gamePhysics->StopActor(m_pOwner->GetId());
    }

}
