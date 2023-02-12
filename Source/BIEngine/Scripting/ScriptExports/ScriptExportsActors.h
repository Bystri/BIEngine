#pragma once

#include <memory>

#include <pybind11/embed.h>

#include "../../EngineCore/GameApp.h"
#include "../../Actors/CameraComponent.h"
#include "../../Actors/PhysicsComponent.h"
#include "../../Actors/PhysicsTriggerComponent.h"
#include "../../Actors/RenderComponent.h"
#include "../../Actors/ScriptComponent.h"

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(BIEActor, m)
{
	py::class_<BIEngine::ActorComponent, std::shared_ptr<BIEngine::ActorComponent>>(m, "ActorComponent")
		.def("GetComponentId", &BIEngine::ActorComponent::GetComponentId);
    
    py::class_<BIEngine::Actor, std::shared_ptr<BIEngine::Actor>>(m, "Actor")
        .def("GetId", &BIEngine::Actor::GetId)
        .def("GetName", &BIEngine::Actor::GetName)
        .def("GetComponent", [](std::shared_ptr<BIEngine::Actor>& self, const std::string& componentId) {return self->GetComponent<BIEngine::ActorComponent>(componentId).lock(); });
    
    py::class_<BIEngine::CameraComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::CameraComponent>>(m, "CameraComponent");
    
    py::class_<BIEngine::PhysicsComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::PhysicsComponent>>(m, "PhysicsComponent")
        .def("ApplyForce", &BIEngine::PhysicsComponent::ApplyForce)
        .def("ApplyTorque", &BIEngine::PhysicsComponent::ApplyTorque)
        .def("KinematicMove", &BIEngine::PhysicsComponent::KinematicMove)
        .def("SetVelocity", &BIEngine::PhysicsComponent::SetVelocity)
        .def("GetVelocity", &BIEngine::PhysicsComponent::GetVelocity)
        .def("SetAngularVelocity", &BIEngine::PhysicsComponent::SetAngularVelocity)
        .def("GetAngularVelocity", &BIEngine::PhysicsComponent::GetAngularVelocity)
        .def("Stop", &BIEngine::PhysicsComponent::Stop);
    
    py::class_<BIEngine::PhysicsTriggerComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::PhysicsTriggerComponent>>(m, "PhysicsTriggerComponent")
        .def("GetVelocity", &BIEngine::PhysicsTriggerComponent::GetVelocity)
        .def("SetVelocity", &BIEngine::PhysicsTriggerComponent::SetVelocity)
        .def("SetRotation", &BIEngine::PhysicsTriggerComponent::SetRotation)
        .def("SetPosition", &BIEngine::PhysicsTriggerComponent::SetPosition)
        .def("Stop", &BIEngine::PhysicsTriggerComponent::Stop);

    
    py::class_<BIEngine::BaseRenderComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::BaseRenderComponent>>(m, "BaseRenderComponent");
    py::class_<BIEngine::BoxRenderComponent, BIEngine::BaseRenderComponent, std::shared_ptr<BIEngine::BoxRenderComponent>>(m, "BoxRenderComponent");
    py::class_<BIEngine::SpriteRenderComponent, BIEngine::BaseRenderComponent, std::shared_ptr<BIEngine::SpriteRenderComponent>>(m, "SpriteRenderComponent");
    
    py::class_<BIEngine::ScriptComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::ScriptComponent>>(m, "ScriptComponent")
        .def("GetObject", &BIEngine::ScriptComponent::GetObject);

    py::class_<BIEngine::TransformComponent, BIEngine::ActorComponent, std::shared_ptr<BIEngine::TransformComponent>>(m, "TransformComponent")
        .def("GetPosition", &BIEngine::TransformComponent::GetPosition)
        .def("SetPosition", &BIEngine::TransformComponent::SetPosition)
        .def("GetRotation", &BIEngine::TransformComponent::GetRotation)
        .def("SetRotation", &BIEngine::TransformComponent::SetRotation)
        .def("GetSize", &BIEngine::TransformComponent::GetSize)
        .def("SetSize", &BIEngine::TransformComponent::SetSize);

    m.def("CreateActor", [](const char* actorArchetypeXmlPath)
    {
        std::shared_ptr<BIEngine::XmlExtraData> pActorData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle(actorArchetypeXmlPath)->GetExtra());
        std::shared_ptr<BIEngine::Actor> pActor = BIEngine::g_pApp->m_pGameLogic->CreateActor(pActorData->GetRootElement());

        if (pActor)
        {
            //std::shared_ptr<EvtData_New_Actor> pNewActorEvent(GCC_NEW EvtData_New_Actor(pActor->GetId()));
            //IEventManager::Get()->VQueueEvent(pNewActorEvent);
            return pActor;
        }

        return std::shared_ptr<BIEngine::Actor>();
    });
}