#pragma once

#include "ActorComponent.h"
#include "../Graphics2D/SceneNodes.h"

namespace BIEngine
{

    //Привязывает камеру к актеру
    class CameraComponent : public ActorComponent
    {
    public:
        CameraComponent() {}

        static ComponentId g_CompId;
        virtual ComponentId GetComponentId() const { return CameraComponent::g_CompId; }

        virtual bool Init(tinyxml2::XMLElement* pData);
        virtual void PostInit() override;

        virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

        std::shared_ptr<CameraNode> GetCameraNode() const {return m_pCameraNode;}

    protected:
        virtual std::shared_ptr<CameraNode> CreateCameraNode();

    protected:
        std::shared_ptr<CameraNode> m_pCameraNode;
    };

    static ActorComponent* CreateCameraComponent()
    {
        return new CameraComponent;
    }
}