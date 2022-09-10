#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "../Actors/Actor.h"
#include "../ResourceCache/XmlLoader.h"

namespace BIEngine
{

	class IGamePhysics2D
	{
	public:
		enum class BodyType
		{
			DYNAMIC,
			KINEMATIC,
			STATIC
		};

		virtual ~IGamePhysics2D() { };

		//—лужебные функции
		virtual bool Initialize() = 0;
		virtual void SetGravity(const glm::vec2& gravity) = 0;
		virtual void SyncVisibleScene(const std::map<ActorId, std::shared_ptr<Actor>>& pActorMap) = 0;
		virtual void OnUpdate(float deltaSeconds) = 0;
		
		//»нициализаци€ физических объектов
		virtual void AddCircle(float radius, BodyType bodyType, std::weak_ptr<Actor> actor, const std::string& densityStr, const std::string& physicsMaterial) = 0;
		virtual void AddBox(const glm::vec2& dimensions, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) = 0;
		virtual void AddPointCloud(const glm::vec2* verts, int numPoints, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) = 0;
		virtual void RemoveActor(ActorId id) = 0;

		//¬заимодейтсвие с физическим миром
		virtual void CreateTrigger(std::weak_ptr<Actor> pGameActor, const glm::vec2& dim) = 0;
		virtual void ApplyForce(const glm::vec2& dir, float newtons, ActorId aid) = 0;
		virtual void ApplyTorque(const glm::vec2& dir, float newtons, ActorId aid) = 0;
		virtual bool KinematicMove(ActorId aid, const glm::vec2& position, float rotate) = 0;

		//явные способы задать актерам пространственные характеристики
		virtual void Rotate(ActorId actorId, float angleRadians) = 0;
		virtual float GetOrientation(ActorId actorId) const = 0;
		virtual void StopActor(ActorId actorId) = 0;
		virtual glm::vec2 GetVelocity(ActorId actorId) const = 0;
		virtual void SetVelocity(ActorId actorId, const glm::vec2& vel) = 0;
		virtual float GetAngularVelocity(ActorId actorId) const = 0;
		virtual void SetAngularVelocity(ActorId actorId, float vel) = 0;
		virtual void SetPosition(const ActorId id, const glm::vec2& position) = 0;
		virtual glm::vec2 GetPosition(const ActorId id) const = 0;
	};

	extern IGamePhysics2D* CreateGamePhysics();
	extern IGamePhysics2D* CreateNullPhysics();

}
