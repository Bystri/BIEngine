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

        TransformComponent() : m_position(0.0, 0.0, 0.0), m_size(1.0, 1.0, 1.0), m_rotation(0) { }
        virtual bool Init(tinyxml2::XMLElement* pData);

        virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

        //Задание и получение свойств 
        glm::vec3 GetPosition() const { return m_position; }
        void SetPosition(const glm::vec3& pos) { m_position = pos; }

        glm::vec3 GetRotation() const { return m_rotation; }
        void SetRotation(const glm::vec3& rot) { m_rotation = rot; }

        glm::vec3 GetSize() const { return m_size; }
        void SetSize(const glm::vec3& size) { m_size = size; }

    private:
        glm::vec3 m_position;
        glm::vec3 m_size;
        glm::vec3 m_rotation;
    };

    static ActorComponent* CreateTransformComponent()
    {
        return new TransformComponent;
    }

}
