#include "RenderComponent.h"

#include "../EventManager/EventManager.h"
#include "../EventManager/Events.h"
#include "../Renderer/ShadersLoader.h"
#include "../Graphics/MeshGeometryGenerator.h"
#include "../Actors/TransformComponent.h"
#include "../Utilities/Logger.h"
#include "../Renderer/ImageLoader.h"

namespace BIEngine
{
    ComponentId SpriteRenderComponent::g_CompId = "SpriteRenderComponent";
    ComponentId BoxRenderComponent::g_CompId = "BoxRenderComponent";

    /***********************************************************
    * BaseRenderComponent
    ************************************************************/

    void BaseRenderComponent::PostInit()
    {
        std::shared_ptr<SceneNode> pSceneNode(GetSceneNode());
        std::shared_ptr<EvtData_New_Render_Component> pEvent = std::make_shared< EvtData_New_Render_Component>(m_pOwner->GetId(), pSceneNode);
        EventManager::Get()->TriggerEvent(pEvent);
    }

    std::shared_ptr<SceneNode> BaseRenderComponent::GetSceneNode()
    {
        if (!m_pSceneNode)
            m_pSceneNode = CreateSceneNode();
        return m_pSceneNode;
    }


    /***********************************************************
    * MeshBaseRenderComponent
    ************************************************************/


    bool MeshBaseRenderComponent::Init(tinyxml2::XMLElement* pData)
    {
        if (!BaseRenderComponent::Init(pData)) {
            return false;
        }

        tinyxml2::XMLElement* pShaderProgramInfo = pData->FirstChildElement("ShaderProgram");

        if (pShaderProgramInfo == nullptr) {
            Logger::WriteLog(Logger::LogType::ERROR, "Error while loading RenderComponent for Actor with id: " + std::to_string(m_pOwner->GetId()) + "; No Shader was specified");
            return false;
        }

        const char* vertexShaderPath;
        pShaderProgramInfo->QueryStringAttribute("vertexShaderPath", &vertexShaderPath);

        const char* fragmentShaderPath;
        pShaderProgramInfo->QueryStringAttribute("fragmentShaderPath", &fragmentShaderPath);

        std::shared_ptr<ShaderData> pVertShaderData = std::static_pointer_cast<ShaderData>(ResCache::Get()->GetHandle(vertexShaderPath)->GetExtra());
        std::shared_ptr<ShaderData> pFragShaderxData = std::static_pointer_cast<ShaderData>(ResCache::Get()->GetHandle(fragmentShaderPath)->GetExtra());
        std::shared_ptr<ShaderProgram> pShaderProgram = std::make_shared<ShaderProgram>();
        pShaderProgram->Compile(pVertShaderData->GetShaderIndex(), pFragShaderxData->GetShaderIndex());

        m_pMaterial = std::make_shared<Material>(pShaderProgram);
        m_pMaterial->SetColor(WHITE);

        tinyxml2::XMLElement* pColorElement = pData->FirstChildElement("Color");
        if (pColorElement)
        {
            float r = 0;
            float g = 0;
            float b = 0;
            pColorElement->QueryFloatAttribute("r", &r);
            pColorElement->QueryFloatAttribute("g", &g);
            pColorElement->QueryFloatAttribute("b", &b);
            m_pMaterial->SetColor(Color(r, g, b, 1.0f));
        }

        return true;
    }


    tinyxml2::XMLElement* MeshBaseRenderComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
    {
        tinyxml2::XMLElement* pBaseElement = BaseRenderComponent::GenerateXml(pDoc);

        tinyxml2::XMLElement* pShader = pDoc->NewElement("ShaderProgram");
        pShader->SetAttribute("vertexShaderPath", m_vertexShaderPath.c_str());
        pShader->SetAttribute("fragmentShaderPath", m_fragmentShaderPath.c_str());
        pBaseElement->LinkEndChild(pShader);

        tinyxml2::XMLElement* pColor = pDoc->NewElement("Color");
        pColor->SetAttribute("r", std::to_string(m_pMaterial->GetColor().r).c_str());
        pColor->SetAttribute("g", std::to_string(m_pMaterial->GetColor().g).c_str());
        pColor->SetAttribute("b", std::to_string(m_pMaterial->GetColor().b).c_str());
        pBaseElement->LinkEndChild(pColor);

        return pColor;
    }


    /***********************************************************
    * SpriteRenderComponent
    ************************************************************/

    bool SpriteRenderComponent::Init(tinyxml2::XMLElement* pData)
    {
        if (!MeshBaseRenderComponent::Init(pData)) {
            return false;
        }

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
            auto imgData = std::static_pointer_cast<ImageExtraData>(ResCache::Get()->GetHandle(spritePath)->GetExtra());

            if (imgData == nullptr)
            {
                Logger::WriteLog(Logger::LogType::ERROR, "Error while loading sprite for Actor with id: " + std::to_string(m_pOwner->GetId()));
                m_pSpriteNode.reset();
                return false;
            }

            std::shared_ptr<Texture2D> pTexture = std::make_shared<Texture2D>();
            pTexture->SetInternalFormat(GL_RGBA);
            pTexture->SetImageFormat(GL_RGBA);
            pTexture->Generate(imgData->GetWidth(), imgData->GetHeight(), imgData->GetData());
            m_pSpriteNode->SetSprite(std::make_shared<Sprite>(pTexture));
        }

        return true;
    }

    tinyxml2::XMLElement* SpriteRenderComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
    {
        tinyxml2::XMLElement* pBaseElement = MeshBaseRenderComponent::GenerateXml(pDoc);

        tinyxml2::XMLElement* pSprite = pDoc->NewElement("Sprite");
        pSprite->SetAttribute("path", m_spritePath.c_str());
        pBaseElement->LinkEndChild(pSprite);

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

   /***********************************************************
   * BoxRenderComponent
   ************************************************************/

    bool BoxRenderComponent::Init(tinyxml2::XMLElement* pData)
    {
        if (!MeshBaseRenderComponent::Init(pData)) {
            return false;
        }

        if (m_pModelNode == nullptr)
        {
            m_pModelNode = std::make_shared<Model3dNode>(m_pOwner->GetId(), RenderLayer::OPAQUE);
        }

        tinyxml2::XMLElement* pSizeElement = pData->FirstChildElement("Size");
        if (pSizeElement)
        {
            double w, h, d;
            pSizeElement->QueryDoubleAttribute("w", &w);
            pSizeElement->QueryDoubleAttribute("h", &h);
            pSizeElement->QueryDoubleAttribute("d", &d);

            m_width = (float)w;
            m_height = (float)h;
            m_depth = (float)d;
        }

        tinyxml2::XMLElement* pMaterialElement = pData->FirstChildElement("Material");
        if (pMaterialElement)
        {
            float shininess = 64.0f;
            pMaterialElement->QueryFloatAttribute("shininess", &shininess);
            m_pMaterial->SetShininess(shininess);

            const char* diffuseMapPath;
            pMaterialElement->QueryStringAttribute("diffuseMapPath", &diffuseMapPath);
            m_diffuseMapPath = diffuseMapPath;
            auto diffuseMapImgData = std::static_pointer_cast<ImageExtraData>(ResCache::Get()->GetHandle(m_diffuseMapPath)->GetExtra());

            if (diffuseMapImgData == nullptr)
            {
                Logger::WriteLog(Logger::LogType::ERROR, "Error while loading sprite for Actor with id: " + std::to_string(m_pOwner->GetId()));
                m_pModelNode.reset();
                return false;
            }

            std::shared_ptr<Texture2D> pDiffuseMapTexture = std::make_shared<Texture2D>();
            pDiffuseMapTexture->SetInternalFormat(GL_RGBA);
            pDiffuseMapTexture->SetImageFormat(GL_RGBA);
            pDiffuseMapTexture->Generate(diffuseMapImgData->GetWidth(), diffuseMapImgData->GetHeight(), diffuseMapImgData->GetData());
            m_pMaterial->SetDiffuseMap(pDiffuseMapTexture);

            const char* specularMapPath;
            pMaterialElement->QueryStringAttribute("specularMapPath", &specularMapPath);
            m_specularMapPath = specularMapPath;
            auto specularMapImgData = std::static_pointer_cast<ImageExtraData>(ResCache::Get()->GetHandle(m_specularMapPath)->GetExtra());

            if (specularMapImgData == nullptr)
            {
                Logger::WriteLog(Logger::LogType::ERROR, "Error while loading sprite for Actor with id: " + std::to_string(m_pOwner->GetId()));
                m_pModelNode.reset();
                return false;
            }

            std::shared_ptr<Texture2D> pSpecularMapTexture = std::make_shared<Texture2D>();
            pSpecularMapTexture->SetInternalFormat(GL_RGBA);
            pSpecularMapTexture->SetImageFormat(GL_RGBA);
            pSpecularMapTexture->Generate(specularMapImgData->GetWidth(), specularMapImgData->GetHeight(), specularMapImgData->GetData());
            m_pMaterial->SetSpecularMap(pSpecularMapTexture);

            std::shared_ptr<Mesh> boxMesh = std::make_shared<Mesh>(MeshGeometryGenerator::CreateBox(m_width, m_height, m_depth, 6u));

            m_pModelNode->SetModel(std::make_shared<Model3d>(boxMesh));
        }

        return true;
    }

    tinyxml2::XMLElement* BoxRenderComponent::GenerateXml(tinyxml2::XMLDocument* pDoc)
    {
        tinyxml2::XMLElement* pBaseElement = MeshBaseRenderComponent::GenerateXml(pDoc);

        tinyxml2::XMLElement* pSize = pDoc->NewElement("Size");
        pSize->SetAttribute("w", std::to_string(m_width).c_str());
        pSize->SetAttribute("h", std::to_string(m_height).c_str());
        pSize->SetAttribute("d", std::to_string(m_depth).c_str());
        pBaseElement->LinkEndChild(pSize);

        tinyxml2::XMLElement* pMaterialElement = pDoc->NewElement("Material");
        pMaterialElement->SetAttribute("shininess", m_pMaterial->GetShininess());
        pMaterialElement->SetAttribute("diffuseMapPath", m_diffuseMapPath.c_str());
        pMaterialElement->SetAttribute("specularMapPath", m_specularMapPath.c_str());
        pBaseElement->LinkEndChild(pMaterialElement);

        return pBaseElement;
    }

    std::shared_ptr<SceneNode> BoxRenderComponent::CreateSceneNode()
    {
        if (m_pModelNode == nullptr)
            return std::shared_ptr<SceneNode>();

        std::shared_ptr<TransformComponent> pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
        if (pTransformComponent)
        {
            m_pModelNode->SetTransform(pTransformComponent);
            m_pModelNode->SetMaterial(m_pMaterial);
        }

        return m_pModelNode;
    }
}