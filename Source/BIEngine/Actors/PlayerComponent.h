#pragma once

#include "ActorComponent.h"

namespace BIEngine {

class PlayerComponent : public ActorComponent {
public:
   static const ComponentId g_CompId;

   virtual ComponentId GetComponentId() const { return PlayerComponent::g_CompId; };

public:
   virtual bool Init(tinyxml2::XMLElement* pData) override;

   virtual tinyxml2::XMLElement* GenerateXml(tinyxml2::XMLDocument* pDoc) override;

   void SetPlayerId(const uint32_t playerId) { m_playerId = playerId; }

   uint32_t GetPlayerId() const { return m_playerId; }

private:
   uint32_t m_playerId = 0xffff;
};

static std::unique_ptr<ActorComponent> CreateBIPlayerComponent()
{
   return std::make_unique<PlayerComponent>();
}

} // namespace BIEngine
