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
            pPositionElement->QueryDoubleAttribute("x", &x);
            pPositionElement->QueryDoubleAttribute("y", &y);
            m_position = glm::vec2(x, y);
        }

        tinyxml2::XMLElement* pSizeElement = pData->FirstChildElement("Size");
        if (pSizeElement)
        {
            double w = 0;
            double h = 0;
            pSizeElement->QueryDoubleAttribute("w", &w);
            pSizeElement->QueryDoubleAttribute("h", &h);
            m_size = glm::vec2(w, h);
        }

        tinyxml2::XMLElement* pRotationElement = pData->FirstChildElement("Rotation");
        if (pRotationElement)
        {
            double angle = 0;
            pRotationElement->QueryDoubleAttribute("angle", &angle);
            m_rotation = angle;
        }

        return true;
    }

    tinyxml2::XMLElement* TransformComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
    {
        tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

        tinyxml2::XMLElement* pPosition = pDoc->NewElement("Position");
        pPosition->SetAttribute("x", std::to_string(m_position.x).c_str());
        pPosition->SetAttribute("y", std::to_string(m_position.y).c_str());
        pBaseElement->LinkEndChild(pPosition);

        tinyxml2::XMLElement* pSize = pDoc->NewElement("Size");
        pSize->SetAttribute("w", std::to_string(m_size.x).c_str());
        pSize->SetAttribute("h", std::to_string(m_size.y).c_str());
        pBaseElement->LinkEndChild(pSize);

        tinyxml2::XMLElement* pRotation = pDoc->NewElement("Rotation");
        pRotation->SetAttribute("angle", std::to_string(m_rotation).c_str());
        pBaseElement->LinkEndChild(pRotation);

        return pBaseElement;
    }
}


