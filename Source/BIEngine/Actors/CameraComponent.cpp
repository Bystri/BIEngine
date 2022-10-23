#include "CameraComponent.h"

#include "../EventManager/EventManager.h"
#include "../EventManager/Events.h"

namespace BIEngine
{
    ComponentId CameraComponent::g_CompId = "CameraComponent";

    bool CameraComponent::Init(tinyxml2::XMLElement* pData)
    {
        assert(pData);

        if (m_pCameraNode == nullptr)
        {
            m_pCameraNode = std::make_shared<CameraNode>();
        }

        tinyxml2::XMLElement* pShape = pData->FirstChildElement("Projection");
        if (pShape)
        {
            std::string shapeStr = pShape->FirstChild()->Value();
            if (shapeStr == "Ortho")
                m_projType = CameraNode::ProjectionType::ORTHO;
            else if (shapeStr == "Perspective")
                m_projType = CameraNode::ProjectionType::PERSPECTIVE;
        }

        m_pCameraNode->SetProjType(m_projType);

        return true;
    }

    void CameraComponent::PostInit()
    {
        CreateCameraNode();
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
        if (!m_pCameraNode)
            return std::shared_ptr<CameraNode>();

        std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
        if (pTransformComponent)
        {
            m_pCameraNode->SetTransform(pTransformComponent);
        }

        return m_pCameraNode;
    }

}