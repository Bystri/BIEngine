#pragma once

#include <glm/glm.hpp>

#include "../../BIEngine/Actors/ActorComponent.h"

class LocomotionInfoComponent : public BIEngine::ActorComponent {
public:
   static const BIEngine::ComponentId g_CompId;


   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   virtual BIEngine::ComponentId GetComponentId() const override { return LocomotionInfoComponent::g_CompId; };

   void SetInputVel(const glm::vec3& inputVel) { m_inputVelVec = inputVel; }

   const glm::vec3& GetInputVel() const { return m_inputVelVec; }

   void SetInputDir(const glm::vec2& inputDir) { m_inputDirVec = inputDir; }

   const glm::vec2& GetInputDir() const { return m_inputDirVec; }

   void SetCurrentVel(const glm::vec3& currentVel) { m_currentVelocity = currentVel; }

   const glm::vec3& GetCurrentVel() const { return m_currentVelocity; }

   void SetCurrentDir(const glm::vec2& currentDir) { m_currentDir = currentDir; }

   const glm::vec2& GetCurrentDir() const { return m_currentDir; }

private:
   glm::vec3 m_inputVelVec = glm::vec3(0.0f);
   glm::vec2 m_inputDirVec = glm::vec2(0.0f);

   glm::vec3 m_currentVelocity = glm::vec3(0.0f);
   glm::vec2 m_currentDir = glm::vec2(0.0f);
};

static std::unique_ptr<BIEngine::ActorComponent> CreateLocomotionInfoComponent()
{
   return std::make_unique<LocomotionInfoComponent>();
}
