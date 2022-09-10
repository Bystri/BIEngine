
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "../Physics/Physics.h"
#include "../Utilities/String.h"
#include "../EngineCore/GameApp.h"

namespace BIEngine
{

    const ComponentId PhysicsComponent::g_CompId = "PhysicsComponent";

    PhysicsComponent::PhysicsComponent()
    {
        m_rigidBodyScale = glm::vec2(1.f, 1.f);

        m_shape = Shape::NONE;
        m_bodyType = IGamePhysics2D::BodyType::DYNAMIC;
    }

    PhysicsComponent::~PhysicsComponent()
    {
        m_gamePhysics->RemoveActor(m_pOwner->GetId());
    }

    bool PhysicsComponent::Init(tinyxml2::XMLElement* pData)
    {
        m_gamePhysics = g_pApp->m_pGameLogic->GetGamePhysics();
        if (!m_gamePhysics)
            return false; //Если нет физического движка, то и смысла в дальнейшем использовании компонента тоже нет

        tinyxml2::XMLElement* pShape = pData->FirstChildElement("Shape");
        if (pShape)
        {
            std::string shapeStr = pShape->FirstChild()->Value();
            if (shapeStr == "Circle")
                m_shape = Shape::CIRCLE;
            else if (shapeStr == "Box")
                m_shape = Shape::BOX;
            else if (shapeStr == "PointCloud")
                m_shape = Shape::POINT_CLOUD;
        }

        tinyxml2::XMLElement* pBodyType = pData->FirstChildElement("BodyType");
        if (pBodyType)
        {
            std::string typeStr = pBodyType->FirstChild()->Value();
            if (typeStr == "Dynamic")
                m_bodyType = IGamePhysics2D::BodyType::DYNAMIC;
            else if (typeStr == "Kinematic")
                m_bodyType = IGamePhysics2D::BodyType::KINEMATIC;
            else if (typeStr == "Static")
                m_bodyType = IGamePhysics2D::BodyType::STATIC;
        }

        tinyxml2::XMLElement* pDensity = pData->FirstChildElement("Density");
        if (pDensity)
            m_density = pDensity->FirstChild()->Value();

        tinyxml2::XMLElement* pMaterial = pData->FirstChildElement("PhysicsMaterial");
        if (pMaterial)
            m_material = pMaterial->FirstChild()->Value();

        tinyxml2::XMLElement* pSizeElement = pData->FirstChildElement("Scale");
        if (pSizeElement)
        {
            double w = 0;
            double h = 0;
            pSizeElement->QueryDoubleAttribute("w", &w);
            pSizeElement->QueryDoubleAttribute("h", &h);
            m_rigidBodyScale = glm::vec2(w, h);
        }

        return true;
    }

    void PhysicsComponent::PostInit()
    {
        if (m_pOwner)
        {
            switch (m_shape)
            {
            case Shape::CIRCLE:
                m_gamePhysics->AddCircle((float)m_rigidBodyScale.x, m_bodyType, m_pOwner, m_density, m_material);
                break;
            case Shape::BOX:
                m_gamePhysics->AddBox(m_rigidBodyScale, m_bodyType, m_pOwner, m_density, m_material);
                break;
            case Shape::POINT_CLOUD:
                Logger::WriteLog(Logger::LogType::ERROR, "Not supported yet!");
                break;
            default:
                break;
            }
        }
    }

    tinyxml2::XMLElement* PhysicsComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
    {
        tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

        //Shape
        tinyxml2::XMLElement* pShape = pDoc->NewElement("Shape");
        tinyxml2::XMLText* pShapeText = nullptr;
        switch(m_shape)
        {
        case Shape::CIRCLE:
            pShapeText = pDoc->NewText("Circle");
            break;
        case Shape::BOX:
            pShapeText = pDoc->NewText("Box");
            break;
        case Shape::POINT_CLOUD:
            pShapeText = pDoc->NewText("PointCloud");
            break;
        default:
            Logger::WriteLog(Logger::LogType::ERROR, "Unrecognized phycics shape type in PhysicsComponent for actor with ActorID: " + std::to_string(m_pOwner->GetId()));
        }
        pShape->LinkEndChild(pShapeText);
        pBaseElement->LinkEndChild(pShape);

        //BodyType
        tinyxml2::XMLElement* pBodyType = pDoc->NewElement("BodyType");
        tinyxml2::XMLText* pBodyTypeText = nullptr;
        switch (m_bodyType)
        {
        case IGamePhysics2D::BodyType::DYNAMIC:
            pBodyTypeText = pDoc->NewText("Dynamic");
            break;
        case IGamePhysics2D::BodyType::KINEMATIC:
            pBodyTypeText = pDoc->NewText("Kinematic");
            break;
        case IGamePhysics2D::BodyType::STATIC:
            pBodyTypeText = pDoc->NewText("Static");
            break;
        default:
            Logger::WriteLog(Logger::LogType::ERROR, "Unrecognized phycics body shape type in PhysicsComponent for actor with ActorID: " + std::to_string(m_pOwner->GetId()));
        }
        pBodyType->LinkEndChild(pBodyTypeText);
        pBaseElement->LinkEndChild(pBodyType);

        //Density
        tinyxml2::XMLElement* pDensity = pDoc->NewElement("Density");
        tinyxml2::XMLText* pDensityText = pDoc->NewText(m_density.c_str());
        pDensity->LinkEndChild(pDensityText);
        pBaseElement->LinkEndChild(pDensity);

        //PhysicsMaterial
        tinyxml2::XMLElement* pPhysicsMaterial = pDoc->NewElement("PhysicsMaterial");
        tinyxml2::XMLText* pPhysicsMaterialText = pDoc->NewText(m_material.c_str());
        pPhysicsMaterial->LinkEndChild(pPhysicsMaterialText);
        pBaseElement->LinkEndChild(pPhysicsMaterial);

        tinyxml2::XMLElement* pScale = pDoc->NewElement("Scale");
        pScale->SetAttribute("w", std::to_string(m_rigidBodyScale.x).c_str());
        pScale->SetAttribute("h", std::to_string(m_rigidBodyScale.y).c_str());
        pBaseElement->LinkEndChild(pScale);

        return pBaseElement;
    }

    void PhysicsComponent::ApplyForce(const glm::vec2& direction, float forceNewtons)
    {
        m_gamePhysics->ApplyForce(direction, forceNewtons, m_pOwner->GetId());
    }

    void PhysicsComponent::ApplyTorque(const glm::vec2& direction, float forceNewtons)
    {
        m_gamePhysics->ApplyTorque(direction, forceNewtons, m_pOwner->GetId());
    }

    bool PhysicsComponent::KinematicMove(const glm::vec2& position, float rotate)
    {
        return m_gamePhysics->KinematicMove(m_pOwner->GetId(), position, rotate);
    }

    glm::vec2 PhysicsComponent::GetVelocity()
    {
        return m_gamePhysics->GetVelocity(m_pOwner->GetId());
    }

    void PhysicsComponent::SetVelocity(const glm::vec2& velocity)
    {
        m_gamePhysics->SetVelocity(m_pOwner->GetId(), velocity);
    }

    void PhysicsComponent::Rotate(float angleRadians)
    {
        m_gamePhysics->Rotate(m_pOwner->GetId(), angleRadians);
    }

    void PhysicsComponent::SetPosition(float x, float y)
    {
        m_gamePhysics->SetPosition(m_pOwner->GetId(), glm::vec2(x, y));
    }

    void PhysicsComponent::Stop()
    {
        return m_gamePhysics->StopActor(m_pOwner->GetId());
    }

}
