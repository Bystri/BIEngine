#pragma once

#include "ActorComponent.h"

#include "../Graphics/SpriteNode.h"
#include "../Graphics/Model3dNode.h"

namespace BIEngine
{
    //Данный компонент отвечает за регестрацию на сцене объекта отрисовки. Делается это с помощью отрпавки сообщения, которое прослушивает сама сцена
    class BaseRenderComponent : public ActorComponent
    {
    protected:
        std::shared_ptr<SceneNode> m_pSceneNode;

    public:
        BaseRenderComponent() 
        {
        }

        virtual bool Init(tinyxml2::XMLElement* pData);
        virtual void PostInit() override;

        virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;
    
    protected:
        //В данном методе дочерние классы создают особые узлы сцены под свой заданный тип компонента актера
        virtual std::shared_ptr<SceneNode> CreateSceneNode() = 0; 

    private:
        virtual std::shared_ptr<SceneNode> GetSceneNode();

    protected:
        std::shared_ptr<Material> m_pMaterial;
    };


    //Создает и регестрирует в сцене спрайт для отрисовки актера
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
        std::shared_ptr<SpriteNode> m_pSpriteNode; //Узел на сцене, который отвечает за отрисовку этого компонента
        std::string m_spritePath; //Сохраняем путь к спрайту, из которого мы делаем текстуру, чтобы потом вставить его в XML, если понадобится
    };

    static ActorComponent* CreateSpriteRenderComponent()
    {
        return new SpriteRenderComponent;
    }

    //Создает и регестрирует в сцене меш сферы для отрисовки
    class BoxRenderComponent : public BaseRenderComponent
    {
    public:
        BoxRenderComponent() : BaseRenderComponent(), m_pModelNode(nullptr), m_width(1.0f), m_height(1.0f), m_depth(1.0f) {}

        static ComponentId g_CompId;
        virtual ComponentId GetComponentId() const { return BoxRenderComponent::g_CompId; }

        virtual bool Init(tinyxml2::XMLElement* pData);

        virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

    protected:
        virtual std::shared_ptr<SceneNode> CreateSceneNode();

    protected:
        std::shared_ptr<Model3dNode> m_pModelNode; //Узел на сцене, который отвечает за отрисовку этого компонента

        std::string m_texturePath; //Сохраняем путь текстуре, чтобы потом вставить его в XML, если понадобится

        float m_width;
        float m_height;
        float m_depth;
    };

    static ActorComponent* CreateBoxRenderComponent()
    {
        return new BoxRenderComponent;
    }
}