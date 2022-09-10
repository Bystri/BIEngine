#pragma once

#include "ActorComponent.h"

#include "../Graphics2D/Opengl/SpriteNode.h"
#include "../Graphics2D/Opengl/TextureLoaderOpenGL.h"

namespace BIEngine
{
    //������ ��������� �������� �� ����������� �� ����� ������� ���������. �������� ��� � ������� �������� ���������, ������� ������������ ���� �����
    class BaseRenderComponent : public ActorComponent
    {
    protected:
        std::shared_ptr<SceneNode> m_pSceneNode;

    public:
        BaseRenderComponent() 
        {
            m_pMaterial = std::make_shared<Material>();
            m_pMaterial->SetColor(WHITE);
        }

        virtual bool Init(tinyxml2::XMLElement* pData);
        virtual void PostInit() override;

        virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;
    
    protected:
        //� ������ ������ �������� ������ ������� ������ ���� ����� ��� ���� �������� ��� ���������� ������
        virtual std::shared_ptr<SceneNode> CreateSceneNode() = 0; 

    private:
        virtual std::shared_ptr<SceneNode> GetSceneNode();

    protected:
        std::shared_ptr<Material> m_pMaterial;
    };


    //������� � ������������ � ����� ������ ��� ��������� ������
    class SpriteRenderComponent : public BaseRenderComponent
    {
    public:
        SpriteRenderComponent() : BaseRenderComponent(), m_pSpriteNode(nullptr) {}

        static ComponentId g_CompId;
        virtual ComponentId GetComponentId() const { return SpriteRenderComponent::g_CompId; }

        virtual bool Init(tinyxml2::XMLElement* pData);

        virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

    protected:
        virtual std::shared_ptr<SceneNode> CreateSceneNode();

    protected:
        std::shared_ptr<SpriteNode> m_pSpriteNode; //���� �� �����, ������� �������� �� ��������� ����� ����������
        std::string m_spritePath; //��������� ���� � �������, �� �������� �� ������ ��������, ����� ����� �������� ��� � XML, ���� �����������
    };

    static ActorComponent* CreateSpriteRenderComponent()
    {
        return new SpriteRenderComponent;
    }
}