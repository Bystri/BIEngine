#pragma once

#include "ActorComponent.h"

#include <glm/glm.hpp>

namespace BIEngine
{

    //Компонент, отвечающий за положение и размер актера
    class TransformComponent : public ActorComponent
    {
    public:
        static const ComponentId g_CompId;
        virtual ComponentId GetComponentId() const { return TransformComponent::g_CompId; };

        TransformComponent() : m_position(0.0, 0.0), m_size(1.0, 1.0), m_rotation(0) { }
        virtual bool Init(tinyxml2::XMLElement* pData);

        virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

        //Задание и получение свойств 
        glm::vec2 GetPosition() const { return m_position; }
        void SetPosition(const glm::vec2& pos) { m_position = pos; }

        float GetRotation() const { return m_rotation; }
        void SetRotation(float rot) { m_rotation = rot; }

        glm::vec2 GetSize() const { return m_size; }
        void SetSize(const glm::vec2& size) { m_size = size; }

    private:
        glm::vec2 m_position;
        glm::vec2 m_size;
        float m_rotation;
    };

    static ActorComponent* CreateTransformComponent()
    {
        return new TransformComponent;
    }

}
