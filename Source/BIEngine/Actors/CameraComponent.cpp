#include "CameraComponent.h"

#include "../EventManager/EventManager.h"
#include "../EventManager/Events.h"

namespace BIEngine
{
    ComponentId CameraComponent::g_CompId = "CameraComponent";

    bool CameraComponent::Init(tinyxml2::XMLElement* pData)
    {
        assert(pData);

        return true;
    }

    void CameraComponent::PostInit()
    {
        m_pCameraNode = CreateCameraNode();
        std::shared_ptr<EvtData_New_Camera_Component> pEvent = std::make_shared< EvtData_New_Camera_Component>(m_pOwner);
        EventManager::Get()->TriggerEvent(pEvent);
    }

    tinyxml2::XMLElement* CameraComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
    {
        tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

        return pBaseElement;
    }

    std::shared_ptr<CameraNode> CameraComponent::CreateCameraNode()
    {
        auto pCameraNode = std::make_shared<CameraNode>();

        std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
        if (pTransformComponent)
        {
            pCameraNode->SetTransform(pTransformComponent);
        }

        return pCameraNode;
    }

}