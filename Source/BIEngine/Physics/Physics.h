#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "../Actors/Actor.h"
#include "../ResourceCache/XmlLoader.h"

namespace BIEngine
{

	class IGamePhysics
	{
	public:
		enum class BodyType
		{
			DYNAMIC,
			KINEMATIC,
			STATIC
		};

		virtual ~IGamePhysics() { };

		//Служебные функции
		virtual bool Initialize() = 0;
		virtual void SetGravity(const glm::vec3& gravity) = 0;
		virtual void SyncVisibleScene(const std::map<ActorId, std::shared_ptr<Actor>>& pActorMap) = 0;
		virtual void OnUpdate(double dt) = 0;
		
		//Инициализация физических объектов
		virtual void AddCircle(float radius, BodyType bodyType, std::weak_ptr<Actor> actor, const std::string& densityStr, const std::string& physicsMaterial) = 0;
		virtual void AddBox(const glm::vec3& dimensions, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) = 0;
		virtual void AddPointCloud(const glm::vec3* verts, int numPoints, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) = 0;
		virtual void RemoveActor(ActorId id) = 0;

		//Взаимодейтсвие с физическим миром
		virtual void CreateTrigger(std::weak_ptr<Actor> pGameActor, const glm::vec3& dim) = 0;
		virtual void ApplyForce(const glm::vec3& dir, ActorId aid) = 0;
		virtual void ApplyTorque(float torque, ActorId aid) = 0;
		virtual bool KinematicMove(ActorId aid, const glm::vec3& position, float rotate) = 0;

		//Явные способы задать актерам пространственные характеристики
		virtual void Rotate(ActorId actorId, float angleRadians) = 0;
		virtual float GetOrientation(ActorId actorId) const = 0;
		virtual void StopActor(ActorId actorId) = 0;
		virtual glm::vec3 GetVelocity(ActorId actorId) const = 0;
		virtual void SetVelocity(ActorId actorId, const glm::vec3& vel) = 0;
		virtual float GetAngularVelocity(ActorId actorId) const = 0;
		virtual void SetAngularVelocity(ActorId actorId, float vel) = 0;
		virtual void SetPosition(const ActorId id, const glm::vec3& position) = 0;
		virtual glm::vec3 GetPosition(const ActorId id) const = 0;
	};

	extern IGamePhysics* CreateGamePhysics();
	extern IGamePhysics* CreateNullPhysics();

}
