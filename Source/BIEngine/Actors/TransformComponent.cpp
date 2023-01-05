#include "TransformComponent.h"

#include <string>

namespace BIEngine
{

    const ComponentId TransformComponent::g_CompId = "TransformComponent";

    bool TransformComponent::Init(tinyxml2::XMLElement* pData)
    {
        assert(pData);

        tinyxml2::XMLElement* pPositionElement = pData->FirstChildElement("Position");
        if (pPositionElement)
        {
            double x = 0;
            double y = 0;
            double z = 0;
            pPositionElement->QueryDoubleAttribute("x", &x);
            pPositionElement->QueryDoubleAttribute("y", &y);
            pPositionElement->QueryDoubleAttribute("z", &z);
            m_position = glm::vec3(x, y, z);
        }

        tinyxml2::XMLElement* pSizeElement = pData->FirstChildElement("Size");
        if (pSizeElement)
        {
            double w = 0;
            double h = 0;
            double d = 0;
            pSizeElement->QueryDoubleAttribute("w", &w);
            pSizeElement->QueryDoubleAttribute("h", &h);
            pSizeElement->QueryDoubleAttribute("d", &d);
            m_size = glm::vec3(w, h, d);
        }

        tinyxml2::XMLElement* pRotationElement = pData->FirstChildElement("Rotation");
        if (pRotationElement)
        {
            double x = 0;
            double y = 0;
            double z = 0;
            pRotationElement->QueryDoubleAttribute("x", &x);
            pRotationElement->QueryDoubleAttribute("y", &y);
            pRotationElement->QueryDoubleAttribute("z", &z);
            m_rotation = glm::vec3(x, y, z);
        }

        return true;
    }

    tinyxml2::XMLElement* TransformComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
    {
        tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

        tinyxml2::XMLElement* pPosition = pDoc->NewElement("Position");
        pPosition->SetAttribute("x", std::to_string(m_position.x).c_str());
        pPosition->SetAttribute("y", std::to_string(m_position.y).c_str());
        pPosition->SetAttribute("z", std::to_string(m_position.z).c_str());
        pBaseElement->LinkEndChild(pPosition);

        tinyxml2::XMLElement* pSize = pDoc->NewElement("Size");
        pSize->SetAttribute("w", std::to_string(m_size.x).c_str());
        pSize->SetAttribute("h", std::to_string(m_size.y).c_str());
        pSize->SetAttribute("d", std::to_string(m_size.z).c_str());
        pBaseElement->LinkEndChild(pSize);

        tinyxml2::XMLElement* pRotation = pDoc->NewElement("Rotation");
        pRotation->SetAttribute("x", std::to_string(m_position.x).c_str());
        pRotation->SetAttribute("y", std::to_string(m_position.y).c_str());
        pRotation->SetAttribute("z", std::to_string(m_position.z).c_str());
        pBaseElement->LinkEndChild(pRotation);

        return pBaseElement;
    }
}


