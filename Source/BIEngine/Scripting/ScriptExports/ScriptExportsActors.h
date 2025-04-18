#pragma once

#include <memory>

#include <pybind11/embed.h>

#include "../../EngineCore/GameApp.h"
#include "../../Actors/TransformComponent.h"
#include "../../Actors/NavAgentComponent.h"
#include "../../Actors/Physics2DComponent.h"
#include "../../Actors/Physics2DTriggerComponent.h"
#include "../../Actors/Physics3DComponent.h"
#include "../../Actors/RenderComponent.h"
#include "../../Actors/AnimationComponent.h"
#include "../../Actors/ScriptComponent.h"
#include "../../Actors/PlayerComponent.h"

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(BIEActor, m)
{
   py::class_<BIEngine::ActorComponent, std::shared_ptr<BIEngine::ActorComponent>>(m, "ActorComponent")
      .def("GetComponentId", &BIEngine::ActorComponent::GetComponentId);

   py::class_<BIEngine::Actor, BIEngine::PythonStateManager::RawPtrWrapper<BIEngine::Actor>>(m, "Actor")
      .def("GetId", &BIEngine::Actor::GetId)
      .def("GetName", &BIEngine::Actor::GetName)
      .def("GetActorByPath", &BIEngine::Actor::GetActorByPath)
      .def("GetComponent", [](std::shared_ptr<BIEngine::Actor>& self, const std::string& componentId) { return self->GetComponent<BIEngine::ActorComponent>(componentId).lock(); })
      .def("SetActivate", &BIEngine::Actor::SetActivate);

   py::class_<BIEngine::NavAgentComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::NavAgentComponent>>(m, "NavAgentComponent")
      .def("SetDestination", &BIEngine::NavAgentComponent::SetDestination)
      .def("GetVelocity", &BIEngine::NavAgentComponent::GetVelocity);

   py::class_<BIEngine::Physics2DComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::Physics2DComponent>>(m, "Physics2DComponent")
      .def("ApplyForce", &BIEngine::Physics2DComponent::ApplyForce)
      .def("ApplyTorque", &BIEngine::Physics2DComponent::ApplyTorque)
      .def("KinematicMove", &BIEngine::Physics2DComponent::KinematicMove)
      .def("SetVelocity", &BIEngine::Physics2DComponent::SetVelocity)
      .def("GetVelocity", &BIEngine::Physics2DComponent::GetVelocity)
      .def("SetAngularVelocity", &BIEngine::Physics2DComponent::SetAngularVelocity)
      .def("GetAngularVelocity", &BIEngine::Physics2DComponent::GetAngularVelocity)
      .def("Stop", &BIEngine::Physics2DComponent::Stop);

   py::class_<BIEngine::Physics2DTriggerComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::Physics2DTriggerComponent>>(m, "Physics2DTriggerComponent")
      .def("GetVelocity", &BIEngine::Physics2DTriggerComponent::GetVelocity)
      .def("SetVelocity", &BIEngine::Physics2DTriggerComponent::SetVelocity)
      .def("SetRotation", &BIEngine::Physics2DTriggerComponent::Rotate)
      .def("SetPosition", &BIEngine::Physics2DTriggerComponent::SetPosition)
      .def("Stop", &BIEngine::Physics2DTriggerComponent::Stop);

   py::class_<BIEngine::Physics3DComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::Physics3DComponent>>(m, "Physics3DComponent")
      .def("ApplyForce", &BIEngine::Physics3DComponent::ApplyForce)
      .def("ApplyTorque", &BIEngine::Physics3DComponent::ApplyTorque)
      .def("KinematicMove", &BIEngine::Physics3DComponent::KinematicMove)
      .def("SetVelocity", &BIEngine::Physics3DComponent::SetVelocity)
      .def("GetVelocity", &BIEngine::Physics3DComponent::GetVelocity)
      .def("SetAngularVelocity", &BIEngine::Physics3DComponent::SetAngularVelocity)
      .def("GetAngularVelocity", &BIEngine::Physics3DComponent::GetAngularVelocity)
      .def("Stop", &BIEngine::Physics3DComponent::Stop);


   py::class_<BIEngine::BoxRenderComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::BoxRenderComponent>>(m, "BoxRenderComponent");
   py::class_<BIEngine::SphereRenderComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::SphereRenderComponent>>(m, "SphereRenderComponent");
   py::class_<BIEngine::SpriteRenderComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::SpriteRenderComponent>>(m, "SpriteRenderComponent");

   py::class_<BIEngine::ScriptComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::ScriptComponent>>(m, "ScriptComponent")
      .def("GetObject", &BIEngine::ScriptComponent::GetObject);

   py::class_<BIEngine::TransformComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::TransformComponent>>(m, "TransformComponent")
      .def("GetPosition", &BIEngine::TransformComponent::GetPosition)
      .def("SetPosition", &BIEngine::TransformComponent::SetPosition)
      .def("GetRotation", &BIEngine::TransformComponent::GetRotation)
      .def("SetRotation", &BIEngine::TransformComponent::SetRotation)
      .def("GetDir", &BIEngine::TransformComponent::GetDir)
      .def("GetSize", &BIEngine::TransformComponent::GetSize)
      .def("SetSize", &BIEngine::TransformComponent::SetSize);

   py::class_<BIEngine::AnimationComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::AnimationComponent>>(m, "AnimationComponent")
      .def("PlayAnimation", &BIEngine::AnimationComponent::PlayAnimation)
      .def("Stop", &BIEngine::AnimationComponent::Stop);

   py::class_<BIEngine::PlayerComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::PlayerComponent>>(m, "PlayerComponent")
      .def("GetPlayerId", &BIEngine::PlayerComponent::GetPlayerId);

   m.def("CreateActor", [](const char* actorArchetypeXmlPath, const glm::vec3& pos, const glm::vec3& rot) {
      std::shared_ptr<BIEngine::XmlExtraData> pActorData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle(actorArchetypeXmlPath)->GetExtra());
      std::shared_ptr<BIEngine::Actor> pActor = BIEngine::g_pApp->m_pGameLogic->CreateActor(pActorData->GetRootElement(), &pos, &rot);

      if (pActor) {
         // std::shared_ptr<EvtData_New_Actor> pNewActorEvent(GCC_NEW EvtData_New_Actor(pActor->GetId()));
         // IEventManager::Get()->VQueueEvent(pNewActorEvent);
         return pActor;
      }

      return std::shared_ptr<BIEngine::Actor>();
   });

   m.def("GetActor", [](BIEngine::ActorId actorId) {
      return BIEngine::g_pApp->m_pGameLogic->GetActor(actorId).get();
   });

   m.def("DestroyActor", [](BIEngine::ActorId actorId) {
      BIEngine::g_pApp->m_pGameLogic->DestroyActor(actorId);
   });
}
