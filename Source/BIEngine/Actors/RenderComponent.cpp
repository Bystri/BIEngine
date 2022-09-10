#include "RenderComponent.h"

#include "../EventManager/EventManager.h"
#include "../EventManager/Events.h"
#include "../Actors/TransformComponent.h"
#include "../Utilities/Logger.h"

namespace BIEngine
{
    ComponentId SpriteRenderComponent::g_CompId = "SpriteRenderComponent";


    bool BaseRenderComponent::Init(tinyxml2::XMLElement* pData)
    {
        tinyxml2::XMLElement* pColorElement = pData->FirstChildElement("Color");
        if (pColorElement)
        {
            float r = 0;
            float g = 0;
            float b = 0;
            pColorElement->QueryFloatAttribute("r", &r);
            pColorElement->QueryFloatAttribute("g", &g);
            pColorElement->QueryFloatAttribute("b", &b);
            m_pMaterial->SetColor(Color(r, g, b));
        }

        return true;
    }

    void BaseRenderComponent::PostInit()
    {
        std::shared_ptr<SceneNode> pSceneNode(GetSceneNode());
        std::shared_ptr<EvtData_New_Render_Component> pEvent = std::make_shared< EvtData_New_Render_Component>(m_pOwner->GetId(), pSceneNode);
        EventManager::Get()->TriggerEvent(pEvent);
    }

    tinyxml2::XMLElement* BaseRenderComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
    {
        tinyxml2::XMLElement* pColor = pDoc->NewElement("Color");
        pColor->SetAttribute("r", std::to_string(m_pMaterial->GetColor().r).c_str());
        pColor->SetAttribute("g", std::to_string(m_pMaterial->GetColor().g).c_str());
        pColor->SetAttribute("b", std::to_string(m_pMaterial->GetColor().b).c_str());

        return pColor;
    }

    std::shared_ptr<SceneNode> BaseRenderComponent::GetSceneNode()
    {
        if (!m_pSceneNode)
            m_pSceneNode = CreateSceneNode();
        return m_pSceneNode;
    }

    bool SpriteRenderComponent::Init(tinyxml2::XMLElement* pData)
    {
        BaseRenderComponent::Init(pData);
        assert(pData);

        if (m_pSpriteNode == nullptr)
        {
            m_pSpriteNode = std::make_shared<SpriteNode>(m_pOwner->GetId(), RenderLayer::OPAQUE);
        }

        tinyxml2::XMLElement* pSpriteElement = pData->FirstChildElement("Sprite");
        if (pSpriteElement)
        {
            const char* spritePath;
            pSpriteElement->QueryStringAttribute("path", &spritePath);
            m_spritePath = spritePath;
            auto texData = std::static_pointer_cast<TextureExtraData>(ResCache::Get()->GetHandle(spritePath)->GetExtra());

            if (texData == nullptr)
            {
                Logger::WriteLog(Logger::LogType::ERROR, "Error while loading sprite for Actor with id: " + std::to_string(m_pOwner->GetId()));
                m_pSpriteNode.reset();
                return false;
            }

            m_pSpriteNode->SetTexture(texData->GetTexture());
        }
        else
        {
            Logger::WriteLog(Logger::LogType::ERROR, "There is no Sprite element in SpriteRenderComponent for Actor with id: " + std::to_string(m_pOwner->GetId()));
            m_pSpriteNode.reset();
            return false;
        }

        return true;
    }

    tinyxml2::XMLElement* SpriteRenderComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
    {
        tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(GetComponentId().c_str());

        tinyxml2::XMLElement* pSprite = pDoc->NewElement("Sprite");
        pSprite->SetAttribute("path", m_spritePath.c_str());
        pBaseElement->LinkEndChild(pSprite);

        tinyxml2::XMLElement* pColor = BaseRenderComponent::GenerateXml(pDoc);
        pBaseElement->LinkEndChild(pColor);

        return pBaseElement;
    }

    std::shared_ptr<SceneNode> SpriteRenderComponent::CreateSceneNode()
    {
        if (m_pSpriteNode == nullptr)
            return std::shared_ptr<SceneNode>();

        std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
        if (pTransformComponent)
        {
            m_pSpriteNode->SetTransform(pTransformComponent);
            m_pSpriteNode->SetMaterial(m_pMaterial);
        }

        return m_pSpriteNode;
    }

}