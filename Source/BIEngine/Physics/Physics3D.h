#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "../Actors/Actor.h"
#include "../Utilities/GameTimer.h"
#include "../ResourceCache/XmlLoader.h"

namespace BIEngine {

class IGamePhysics3D {
public:
   struct RaycastInfo {
      bool hasHit = false;
      glm::vec3 hitPosition;
      std::shared_ptr<Actor> hitActor = nullptr;
   };

   enum class BodyType {
      DYNAMIC,   // Учавствуют в полноценной физической симуляции
      KINEMATIC, // Могут двигаться благодаря прямым командам пользователя, но эффект от взаимодейтсвия будет односторонним - динамические объект будут реагировать на движение кинематических объектов, но не наоборот
      STATIC     // Вообще не учавствуют в физической симуляции (например, триггеры)
   };

   virtual ~IGamePhysics3D(){};

   // Служебные функции
   virtual bool Initialize() = 0;
   virtual void SetGravity(const glm::vec3& gravity) = 0;
   virtual void SyncVisibleScene(const std::map<ActorId, std::shared_ptr<Actor>>& pActorMap) = 0;
   virtual void OnUpdate(const GameTimer& gt) = 0;
   virtual void DrawRenderDiagnostics() = 0;

   // Инициализация физических объектов
   virtual void AddSphere(float radius, BodyType bodyType, std::weak_ptr<Actor> actor, const std::string& densityStr, const std::string& physicsMaterial) = 0;
   virtual void AddBox(const glm::vec3& dimensions, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) = 0;
   virtual void AddPointCloud(const glm::vec3* verts, int numPoints, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) = 0;
   virtual void RemoveActor(ActorId id) = 0;

   // Взаимодейтсвие с физическим миром
   virtual void CreateTrigger(std::weak_ptr<Actor> pGameActor, const glm::vec3& dim) = 0;
   virtual void ApplyForce(const glm::vec3& forceVec, ActorId aid) = 0;
   virtual void ApplyTorque(const glm::vec3& torque, ActorId aid) = 0;
   virtual bool KinematicMove(ActorId aid, const glm::vec3& position, const glm::vec3& angles) = 0;

   virtual RaycastInfo Raycast(const glm::vec3& from, const glm::vec3& to) = 0;

   // Явные способы задать актерам пространственные характеристики
   virtual void RotateY(ActorId actorId, float angleRadians) = 0;
   virtual float GetOrientationY(ActorId actorId) const = 0;
   virtual void StopActor(ActorId actorId) = 0;
   virtual glm::vec3 GetVelocity(ActorId actorId) const = 0;
   virtual void SetVelocity(ActorId actorId, const glm::vec3& vel) = 0;
   virtual glm::vec3 GetAngularVelocity(ActorId actorId) const = 0;
   virtual void SetAngularVelocity(ActorId actorId, const glm::vec3& vel) = 0;
   virtual void SetPosition(const ActorId id, const glm::vec3& position) = 0;
   virtual glm::vec3 GetPosition(const ActorId id) const = 0;
};

extern IGamePhysics3D* CreateGamePhysics3D();
extern IGamePhysics3D* CreateNullPhysics3D();

} // namespace BIEngine
