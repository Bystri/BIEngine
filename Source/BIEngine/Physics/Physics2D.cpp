#include "Physics2D.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <set>
#include <iterator>
#include <vector>
#include <map>

#include <chipmunk/chipmunk.h>

#include "../Actors/Actor.h"
#include "../Actors/TransformComponent.h"
#include "../EventManager/EventManager.h"
#include "../EventManager/Events.h"
#include "Physics2DEventListener.h"

namespace BIEngine {

// Определение отсутсвующей физи с помощью паттерна "нулевой объект"
class NullPhysics2D : public IGamePhysics2D {
public:
   NullPhysics2D() {}

   virtual ~NullPhysics2D() {}

   virtual bool Initialize() override { return true; }

   virtual void SetGravity(const glm::vec2& gravity) override{};
   virtual void SyncVisibleScene(const std::map<ActorId, std::shared_ptr<Actor>>& actorMap) override{};

   virtual void OnUpdate(double) override {}

   virtual void AddCircle(float radius, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) override {}

   virtual void AddBox(const glm::vec2& dimensions, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) override {}

   virtual void AddPointCloud(const glm::vec2* verts, int numPoints, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) override {}

   virtual void RemoveActor(ActorId id) override {}

   virtual void CreateTrigger(std::weak_ptr<Actor> pGameActor, const glm::vec2& dim) override {}

   virtual void ApplyForce(const glm::vec2& forceVec, ActorId aid) override {}

   virtual void ApplyTorque(float torque, ActorId aid) override {}

   virtual bool KinematicMove(ActorId aid, const glm::vec2& position, float angle) override { return true; }

   virtual void Rotate(ActorId actorId, float angleRadians) override {}

   virtual float GetOrientation(ActorId actorId) const override { return 0.0; }

   virtual void StopActor(ActorId actorId) override {}

   virtual glm::vec2 GetVelocity(ActorId actorId) const override { return glm::vec2(); }

   virtual void SetVelocity(ActorId actorId, const glm::vec2& vel) override {}

   virtual float GetAngularVelocity(ActorId actorId) const override { return 0.0f; }

   virtual void SetAngularVelocity(ActorId actorId, float vel) override {}

   virtual void SetPosition(const ActorId id, const glm::vec2& position) override {}

   virtual glm::vec2 GetPosition(const ActorId id) const override { return glm::vec2(); }
};

IGamePhysics2D* CreateNullPhysics2D()
{
   IGamePhysics2D* pGamePhysics2D = new NullPhysics2D();
   assert(pGamePhysics2D);
   return pGamePhysics2D;
}

// Данные о материале, из которого "изготовлен объект". Загружается из компонента актера, отвечающего за физику
struct MaterialData {
   float m_restitution;
   float m_friction;

   MaterialData(float restitution, float friction)
   {
      m_restitution = restitution;
      m_friction = friction;
   }

   MaterialData(const MaterialData& other)
   {
      m_restitution = other.m_restitution;
      m_friction = other.m_friction;
   }
};

class Physics2D : public IGamePhysics2D {
public:
   Physics2D();
   virtual ~Physics2D();

   Physics2D(const Physics2D& orig) = delete;
   Physics2D& operator=(const Physics2D& rhs) = delete;

   // Инициализация физического движка и загузка информации о физических материалах из XML-файла
   virtual bool Initialize() override;
   // Задает двумерный вектор гравитации.
   virtual void SetGravity(const glm::vec2& gravity) override;
   // Сравнивает сохраненное местоположение актеров и то местоположение, которое хранится внутри физической симуляции.
   // В случае различия, местоположение актера обновляется
   virtual void SyncVisibleScene(const std::map<ActorId, std::shared_ptr<Actor>>& actorMap) override;
   // Шаг симуляции
   virtual void OnUpdate(double dt) override;

   // Добавляет физический объект в виде круга в физическую симуляцию
   virtual void AddCircle(float radius, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) override;
   // Добавляет физический объект в виде прямоугольника в физическую симуляцию
   virtual void AddBox(const glm::vec2& dimensions, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) override;
   virtual void AddPointCloud(const glm::vec2* verts, int numPoints, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) override;
   virtual void RemoveActor(ActorId id) override;

   // Добавляет триггер-объект с нулевой физикой в симуляцию
   virtual void CreateTrigger(std::weak_ptr<Actor> pGameActor, const glm::vec2& dim) override;
   // Применяет силу к объекту
   virtual void ApplyForce(const glm::vec2& forceVec, ActorId aid) override;
   // Применить момент силы к объекту
   virtual void ApplyTorque(float torque, ActorId aid) override;

   // Напрямую задает положение и поворот физического объекта.
   // Следует быть с этим аккуратнее, так как данная процедура способна сломать физическую симуляцию
   virtual bool KinematicMove(ActorId aid, const glm::vec2& position, float angle) override;
   // Напрямую задает поворот физического объекта.
   // Следует быть с этим аккуратнее, так как данная процедура способна сломать физическую симуляцию
   virtual void Rotate(ActorId actorId, float const deltaAngleRadians) override;
   virtual float GetOrientation(ActorId actorId) const override;
   // Приравниваем скорость актера к нулевому вектору
   virtual void StopActor(ActorId actorId) override;
   virtual glm::vec2 GetVelocity(ActorId actorId) const override;
   virtual void SetVelocity(ActorId actorId, const glm::vec2& vel) override;
   virtual float GetAngularVelocity(ActorId actorId) const override;
   virtual void SetAngularVelocity(ActorId actorId, float vel) override;
   virtual void SetPosition(const ActorId id, const glm::vec2& position) override;
   virtual glm::vec2 GetPosition(const ActorId id) const override;

private:
   // Инициализация и работа с физическими материалами
   void LoadXml(tinyxml2::XMLElement* pRoot);
   float LookupSpecificGravity(const std::string& densityStr);
   MaterialData LookupMaterialData(const std::string& materialStr);

   cpBody* FindChipmunkRigidBody(ActorId const id) const;
   ActorId FindActorID(cpBody const*) const;

   // Работа с коллизией объектов
   void RemoveCollisionObject(cpBody* removeMe);
   void SendCollisionPairAddEvent(cpArbiter const* arb, cpShape const* shape0, cpShape const* shape1);
   void SendCollisionPairRemoveEvent(cpArbiter const* arb, cpShape const* shape0, cpShape const* shape1);
   static cpBool ChipmunkBeginCollisionCallback(cpArbiter* arb, cpSpace* space, void* data);
   static void ChipmunkSeparateCollisionCallback(cpArbiter* arb, cpSpace* space, void* data);

   // Удаление уничтоженного актера из симуляции
   void DestroyActorDelegate(IEventDataPtr pEventData);

private:
   cpSpace* m_cpSpace;

   // Физические свойства материалов
   typedef std::map<std::string, float> DensityTable;
   typedef std::map<std::string, MaterialData> MaterialTable;
   DensityTable m_densityTable;
   MaterialTable m_materialTable;

   typedef std::map<ActorId, cpBody* const> ActorIDToRigidBodyMap;
   ActorIDToRigidBodyMap m_actorIdToRigidBody;

   typedef std::map<cpBody const*, ActorId> RigidBodyToActorIDMap;
   RigidBodyToActorIDMap m_rigidBodyToActorId;
};

Physics2D::Physics2D()
   : IGamePhysics2D()
{
   EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &Physics2D::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);
}

Physics2D::~Physics2D()
{
   EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &Physics2D::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);

   cpSpaceFree(m_cpSpace);
}

bool Physics2D::Initialize()
{
   auto xmlExtraData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle("config/physics.xml")->GetExtra());
   LoadXml(xmlExtraData->GetRootElement());

   m_cpSpace = cpSpaceNew();
   cpCollisionHandler* pCollisionHandler = cpSpaceAddDefaultCollisionHandler(m_cpSpace);

   // Назначаем наши функции для обработки начала и конца столкновений
   pCollisionHandler->beginFunc = ChipmunkBeginCollisionCallback;
   pCollisionHandler->separateFunc = ChipmunkSeparateCollisionCallback;
   cpSpaceSetUserData(m_cpSpace, this);

   return true;
}

void Physics2D::SetGravity(const glm::vec2& gravity)
{
   cpVect grav = cpv(gravity.x, gravity.y);
   cpSpaceSetGravity(m_cpSpace, grav);
}

void Physics2D::SyncVisibleScene(const std::map<ActorId, std::shared_ptr<Actor>>& actorMap)
{
   for (ActorIDToRigidBodyMap::const_iterator it = m_actorIdToRigidBody.begin();
        it != m_actorIdToRigidBody.end();
        ++it) {
      ActorId const id = it->first;

      std::shared_ptr<Actor> pGameActor = actorMap.find(id)->second;
      if (pGameActor) {
         std::shared_ptr<TransformComponent> pTransformComponent = pGameActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
         if (pTransformComponent) {
            const cpVect cpPos = cpBodyGetPosition(it->second);
            const glm::vec3 pos = glm::vec3(cpPos.x, cpPos.y, 0.0);
            const float rot = cpBodyGetAngle(it->second);

            if (pTransformComponent->GetPosition() != pos || pTransformComponent->GetRotation().z != rot) {
               pTransformComponent->SetPosition(pos);
               pTransformComponent->SetRotation(glm::vec3(0.0f, 0.0f, rot));
               std::shared_ptr<EvtData_Move_Actor> pEvent = std::make_shared<EvtData_Move_Actor>(id, pos, glm::vec3(0.0f, 0.0f, rot));
               EventManager::Get()->QueueEvent(pEvent);
            }
         }
      }
   }
}

void Physics2D::OnUpdate(double dt)
{
   cpSpaceStep(m_cpSpace, dt);
}

void Physics2D::AddCircle(float radius, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial)
{
   std::shared_ptr<Actor> pStrongActor = gameActor.lock();
   if (!pStrongActor)
      return;

   ActorId actorID = pStrongActor->GetId();
   assert(m_actorIdToRigidBody.find(actorID) == m_actorIdToRigidBody.end() && "Actor with more than one physics body?");

   float specificGravity = LookupSpecificGravity(densityStr);
   MaterialData material(LookupMaterialData(physicsMaterial));

   const float area = M_PI * radius * radius;
   const float mass = area * specificGravity;
   const float moment = cpMomentForCircle(mass, 0, radius, cpvzero);

   glm::vec3 position;
   float rotation = 0.0f;
   std::shared_ptr<TransformComponent> pTransformComponent = pStrongActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   assert(pTransformComponent);
   if (pTransformComponent) {
      position = pTransformComponent->GetPosition();
      rotation = pTransformComponent->GetRotation().z;
   } else
      return;

   cpBody* pBody;

   switch (bodyType) {
      case BodyType::DYNAMIC:
         pBody = cpBodyNew(mass, moment);
         break;
      case BodyType::KINEMATIC:
         pBody = cpBodyNewKinematic();
         break;
      case BodyType::STATIC:
         pBody = cpBodyNewStatic();
         break;
      default:
         Logger::WriteLog(Logger::LogType::ERROR, "Unsupported body type");
         return;
   }

   pBody = cpSpaceAddBody(m_cpSpace, pBody);
   cpBodySetPosition(pBody, cpv(position.x, position.y));
   cpBodySetAngle(pBody, (cpFloat)rotation);
   cpShape* pShape = cpSpaceAddShape(m_cpSpace, cpCircleShapeNew(pBody, radius, cpvzero));
   cpShapeSetFriction(pShape, material.m_friction);
   cpShapeSetElasticity(pShape, material.m_restitution);
   cpShapeSetDensity(pShape, specificGravity);

   m_actorIdToRigidBody.insert(std::make_pair(actorID, pBody));
   m_rigidBodyToActorId[pBody] = actorID;
}

void Physics2D::AddBox(const glm::vec2& dimensions, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial)
{
   std::shared_ptr<Actor> pStrongActor = gameActor.lock();
   if (!pStrongActor)
      return;

   ActorId actorID = pStrongActor->GetId();
   assert(m_actorIdToRigidBody.find(actorID) == m_actorIdToRigidBody.end() && "Actor with more than one physics body?");

   float specificGravity = LookupSpecificGravity(densityStr);
   MaterialData material(LookupMaterialData(physicsMaterial));

   const float area = dimensions.x * dimensions.y;
   const float mass = area * specificGravity;
   const float moment = cpMomentForBox(mass, dimensions.x, dimensions.y);

   glm::vec3 position;
   float rotation = 0.0f;
   std::shared_ptr<TransformComponent> pTransformComponent = pStrongActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   assert(pTransformComponent);
   if (pTransformComponent) {
      position = pTransformComponent->GetPosition();
      rotation = pTransformComponent->GetRotation().z;
   } else
      return;

   cpBody* pBody;

   switch (bodyType) {
      case BodyType::DYNAMIC:
         pBody = cpBodyNew(mass, moment);
         break;
      case BodyType::KINEMATIC:
         pBody = cpBodyNewKinematic();
         break;
      case BodyType::STATIC:
         pBody = cpBodyNewStatic();
         break;
      default:
         Logger::WriteLog(Logger::LogType::ERROR, "Unsupported body type");
         return;
   }

   pBody = cpSpaceAddBody(m_cpSpace, pBody);
   cpBodySetPosition(pBody, cpv(position.x, position.y));
   cpBodySetAngle(pBody, (cpFloat)rotation);
   cpShape* pShape = cpSpaceAddShape(m_cpSpace, cpBoxShapeNew(pBody, dimensions.x, dimensions.y, 0.0f));
   cpShapeSetFriction(pShape, material.m_friction);
   cpShapeSetElasticity(pShape, material.m_restitution);
   cpShapeSetDensity(pShape, specificGravity);

   m_actorIdToRigidBody.insert(std::make_pair(actorID, pBody));
   m_rigidBodyToActorId[pBody] = actorID;
}

// Добавляет физических объект состоящий из произвольного набора точек в физическую симуляцию
void Physics2D::AddPointCloud(const glm::vec2* verts, int numPoints, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial)
{
   std::shared_ptr<Actor> pStrongActor = gameActor.lock();
   if (!pStrongActor)
      return;

   ActorId actorID = pStrongActor->GetId();
   assert(m_actorIdToRigidBody.find(actorID) == m_actorIdToRigidBody.end() && "Actor with more than one physics body?");

   cpVect* cpVerts = new cpVect[numPoints];
   for (int i = 0; i < numPoints; ++i) {
      cpVerts[i] = cpv(verts[i].x, verts[i].y);
   }

   float specificGravity = LookupSpecificGravity(densityStr);
   MaterialData material(LookupMaterialData(physicsMaterial));

   const float area = cpAreaForPoly(numPoints, cpVerts, 0.0f);
   const float mass = area * specificGravity;
   const float moment = cpMomentForPoly(mass, numPoints, cpVerts, cpvzero, 0.0f);

   glm::vec3 position;
   float rotation = 0.0f;
   std::shared_ptr<TransformComponent> pTransformComponent = pStrongActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   assert(pTransformComponent);
   if (pTransformComponent) {
      position = pTransformComponent->GetPosition();
      rotation = pTransformComponent->GetRotation().z;
   } else
      return;


   cpBody* pBody;

   switch (bodyType) {
      case BodyType::DYNAMIC:
         pBody = cpBodyNew(mass, moment);
         break;
      case BodyType::KINEMATIC:
         pBody = cpBodyNewKinematic();
         break;
      case BodyType::STATIC:
         pBody = cpBodyNewStatic();
         break;
      default:
         Logger::WriteLog(Logger::LogType::ERROR, "Unsupported body type");
         return;
   }

   pBody = cpSpaceAddBody(m_cpSpace, pBody);
   cpBodySetPosition(pBody, cpv(position.x, position.y));
   cpBodySetAngle(pBody, (cpFloat)rotation);
   cpShape* pShape = cpSpaceAddShape(m_cpSpace, cpPolyShapeNewRaw(pBody, numPoints, cpVerts, 0.0));
   cpShapeSetFriction(pShape, material.m_friction);
   cpShapeSetElasticity(pShape, material.m_restitution);
   cpShapeSetDensity(pShape, specificGravity);

   delete[] cpVerts;

   m_actorIdToRigidBody.insert(std::make_pair(actorID, pBody));
   m_rigidBodyToActorId[pBody] = actorID;
}

// Удаляет физический объект из симуляции
void Physics2D::RemoveActor(ActorId id)
{
   if (cpBody* const body = FindChipmunkRigidBody(id)) {
      RemoveCollisionObject(body);
      m_actorIdToRigidBody.erase(id);
      m_rigidBodyToActorId.erase(body);
   }
}

void Physics2D::CreateTrigger(std::weak_ptr<Actor> pGameActor, const glm::vec2& dim)
{
   std::shared_ptr<Actor> pStrongActor = pGameActor.lock();
   if (!pStrongActor)
      return;

   ActorId actorID = pStrongActor->GetId();
   assert(m_actorIdToRigidBody.find(actorID) == m_actorIdToRigidBody.end() && "Actor with more than one physics body?");

   glm::vec3 position;

   std::shared_ptr<TransformComponent> pTransformComponent = pStrongActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
   assert(pTransformComponent);
   if (pTransformComponent) {
      position = pTransformComponent->GetPosition();
   } else
      return;

   cpBody* pBody = cpSpaceAddBody(m_cpSpace, cpBodyNewKinematic());
   cpBodySetPosition(pBody, cpv(position.x, position.y));
   cpShape* pShape = cpSpaceAddShape(m_cpSpace, cpBoxShapeNew(pBody, dim.x, dim.y, 0.0f));
   cpShapeSetSensor(pShape, true);


   m_actorIdToRigidBody.insert(std::make_pair(actorID, pBody));
   m_rigidBodyToActorId[pBody] = actorID;
}

void Physics2D::ApplyForce(const glm::vec2& dir, ActorId aid)
{
   if (cpBody* const pBody = FindChipmunkRigidBody(aid)) {

      const cpVect force = cpv(dir.x, dir.y);

      cpBodyApplyForceAtLocalPoint(pBody, force, cpvzero);
   }
}

void Physics2D::ApplyTorque(float torque, ActorId aid)
{
   if (cpBody* const pBody = FindChipmunkRigidBody(aid)) {
      cpBodySetTorque(pBody, (cpFloat)torque);
   }
}

bool Physics2D::KinematicMove(ActorId aid, const glm::vec2& position, float angle)
{
   if (cpBody* const pBody = FindChipmunkRigidBody(aid)) {
      cpBodySetPosition(pBody, cpv(position.x, position.y));
      cpBodySetAngle(pBody, angle);
      return true;
   }

   return false;
}

void Physics2D::Rotate(ActorId const actorId, float const deltaAngleRadians)
{
   cpBody* pRigidBody = FindChipmunkRigidBody(actorId);
   assert(pRigidBody);

   cpBodySetAngle(pRigidBody, cpBodyGetAngle(pRigidBody) + deltaAngleRadians);
}

float Physics2D::GetOrientation(ActorId actorId) const
{
   cpBody* pRigidBody = FindChipmunkRigidBody(actorId);
   assert(pRigidBody);

   if (pRigidBody) {
      return cpBodyGetAngle(pRigidBody);
   }

   return std::numeric_limits<float>::max();
}

void Physics2D::StopActor(ActorId actorId)
{
   SetVelocity(actorId, glm::vec3(0.f, 0.f, 0.f));
}

glm::vec2 Physics2D::GetVelocity(ActorId actorId) const
{
   cpBody* pRigidBody = FindChipmunkRigidBody(actorId);
   assert(pRigidBody);
   if (!pRigidBody)
      return glm::vec3();

   cpVect cpVel = cpBodyGetVelocity(pRigidBody);
   return glm::vec2(cpVel.x, cpVel.y);
}

void Physics2D::SetVelocity(ActorId actorId, const glm::vec2& vel)
{
   cpBody* pRigidBody = FindChipmunkRigidBody(actorId);
   assert(pRigidBody);
   if (!pRigidBody)
      return;

   cpBodySetVelocity(pRigidBody, cpv(vel.x, vel.y));
}

float Physics2D::GetAngularVelocity(ActorId actorId) const
{
   cpBody* pRigidBody = FindChipmunkRigidBody(actorId);
   assert(pRigidBody);
   if (!pRigidBody)
      return 0.0f;
   return cpBodyGetAngularVelocity(pRigidBody);
}

void Physics2D::SetAngularVelocity(ActorId actorId, float vel)
{
   cpBody* pRigidBody = FindChipmunkRigidBody(actorId);
   assert(pRigidBody);
   if (!pRigidBody)
      return;

   cpBodySetAngularVelocity(pRigidBody, vel);
}

void Physics2D::SetPosition(const ActorId id, const glm::vec2& position)
{
   cpBody* pRigidBody = FindChipmunkRigidBody(id);
   assert(pRigidBody);
   if (!pRigidBody)
      return;

   cpBodySetPosition(pRigidBody, cpv(position.x, position.y));
}

glm::vec2 Physics2D::GetPosition(const ActorId id) const
{
   cpBody* pRigidBody = FindChipmunkRigidBody(id);
   assert(pRigidBody);
   if (!pRigidBody)
      return glm::vec3();
   cpVect pos = cpBodyGetPosition(pRigidBody);

   return glm::vec2(pos.x, pos.y);
}

void Physics2D::LoadXml(tinyxml2::XMLElement* pRoot)
{
   assert(pRoot);

   tinyxml2::XMLElement* pParentNode = pRoot->FirstChildElement("PhysicsMaterials");
   assert(pParentNode);
   for (tinyxml2::XMLElement* pNode = pParentNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
      double restitution = 0;
      double friction = 0;
      pNode->QueryDoubleAttribute("restitution", &restitution);
      pNode->QueryDoubleAttribute("friction", &friction);
      m_materialTable.insert(std::make_pair(pNode->Value(), MaterialData((float)restitution, (float)friction)));
   }

   pParentNode = pRoot->FirstChildElement("DensityTable");
   assert(pParentNode);
   for (tinyxml2::XMLElement* pNode = pParentNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
      m_densityTable.insert(std::make_pair(pNode->Value(), (float)atof(pNode->FirstChild()->Value())));
   }
}

float Physics2D::LookupSpecificGravity(const std::string& densityStr)
{
   float density = 0;
   auto densityIt = m_densityTable.find(densityStr);
   if (densityIt != m_densityTable.end())
      density = densityIt->second;

   return density;
}

MaterialData Physics2D::LookupMaterialData(const std::string& materialStr)
{
   auto materialIt = m_materialTable.find(materialStr);
   if (materialIt != m_materialTable.end())
      return materialIt->second;
   else
      return MaterialData(0.0, 0.0);
}

cpBody* Physics2D::FindChipmunkRigidBody(ActorId const id) const
{
   ActorIDToRigidBodyMap::const_iterator found = m_actorIdToRigidBody.find(id);
   if (found != m_actorIdToRigidBody.end())
      return found->second;

   return nullptr;
}

ActorId Physics2D::FindActorID(cpBody const* const body) const
{
   RigidBodyToActorIDMap::const_iterator found = m_rigidBodyToActorId.find(body);
   if (found != m_rigidBodyToActorId.end())
      return found->second;

   return ActorId();
}

void Physics2D::RemoveCollisionObject(cpBody* const removeMe)
{
   cpSpaceRemoveBody(m_cpSpace, removeMe);
   auto func = [](cpBody* body, cpShape* shape, void* pData) { cpShapeDestroy(shape); };
   cpBodyEachShape(removeMe, func, nullptr);
   cpBodyDestroy(removeMe);
}

void Physics2D::SendCollisionPairAddEvent(cpArbiter const* arb, cpShape const* shape0, cpShape const* shape1)
{
   // Sensor - это объект без физических свойств, то есть "триггер"
   if (cpShapeGetSensor(shape0) || cpShapeGetSensor(shape1)) {
      cpBody const *triggerBody, *otherBody;

      if (cpShapeGetSensor(shape0)) {
         triggerBody = cpShapeGetBody(shape0);
         otherBody = cpShapeGetBody(shape1);
      } else {
         otherBody = cpShapeGetBody(shape0);
         triggerBody = cpShapeGetBody(shape1);
      }

      std::shared_ptr<EvtData_Phys2DTrigger_Enter> pEvent(new EvtData_Phys2DTrigger_Enter(FindActorID(triggerBody), FindActorID(otherBody)));
      EventManager::Get()->QueueEvent(pEvent);
   } else {
      cpBody const *body0, *body1;
      body0 = cpShapeGetBody(shape0);
      body1 = cpShapeGetBody(shape1);
      ActorId const id0 = FindActorID(body0);
      ActorId const id1 = FindActorID(body1);

      if (id0 == Actor::INVALID_ACTOR_ID || id1 == Actor::INVALID_ACTOR_ID)
         return;

      int numContacts = cpArbiterGetCount(arb);

      std::vector<glm::vec2> collisionPoints;
      const cpVect cpVectNorm = cpArbiterGetNormal(arb);
      glm::vec2 normalForce = glm::vec2(cpVectNorm.x, cpVectNorm.y);
      float friction = cpArbiterGetFriction(arb);

      for (int pointIdx = 0; pointIdx < numContacts; ++pointIdx) {
         const cpVect pointA = cpArbiterGetPointA(arb, pointIdx);
         const cpVect pointB = cpArbiterGetPointB(arb, pointIdx);

         collisionPoints.push_back(glm::vec2(pointA.x, pointA.y));
      }

      IEventDataPtr pEvent = std::make_shared<EvtData_Phys2DCollision>(id0, id1, normalForce, friction, collisionPoints);
      EventManager::Get()->QueueEvent(pEvent);
   }
}

void Physics2D::SendCollisionPairRemoveEvent(cpArbiter const* arb, cpShape const* shape0, cpShape const* shape1)
{
   // Sensor - это объект без физических свойств, то есть "триггер"
   if (cpShapeGetSensor(shape0) || cpShapeGetSensor(shape1)) {
      cpBody const *triggerBody, *otherBody;

      if (cpShapeGetSensor(shape0)) {
         triggerBody = cpShapeGetBody(shape0);
         otherBody = cpShapeGetBody(shape1);
      } else {
         otherBody = cpShapeGetBody(shape0);
         triggerBody = cpShapeGetBody(shape1);
      }

      std::shared_ptr<EvtData_Phys2DTrigger_Leave> pEvent = std::make_shared<EvtData_Phys2DTrigger_Leave>(FindActorID(triggerBody), FindActorID(otherBody));
      EventManager::Get()->QueueEvent(pEvent);
   } else {
      cpBody const *body0, *body1;
      body0 = cpShapeGetBody(shape0);
      body1 = cpShapeGetBody(shape1);
      ActorId const id0 = FindActorID(body0);
      ActorId const id1 = FindActorID(body1);

      if (id0 == Actor::INVALID_ACTOR_ID || id1 == Actor::INVALID_ACTOR_ID)
         return;

      std::shared_ptr<EvtData_Phys2DSeparation> pEvent = std::make_shared<EvtData_Phys2DSeparation>(id0, id1);
      EventManager::Get()->QueueEvent(pEvent);
   }
}

cpBool Physics2D::ChipmunkBeginCollisionCallback(cpArbiter* arb, cpSpace* space, void* data)
{
   assert(cpSpaceGetUserData(space));
   Physics2D* const chipmunkPhysics = static_cast<Physics2D*>(cpSpaceGetUserData(space));

   if (cpArbiterIsFirstContact(arb)) {
      cpShape *a, *b;
      cpArbiterGetShapes(arb, &a, &b);
      chipmunkPhysics->SendCollisionPairAddEvent(arb, a, b);
   }

   return true;
}

void Physics2D::ChipmunkSeparateCollisionCallback(cpArbiter* arb, cpSpace* space, void* data)
{
   assert(cpSpaceGetUserData(space));
   Physics2D* const chipmunkPhysics = static_cast<Physics2D*>(cpSpaceGetUserData(space));

   cpShape *a, *b;
   cpArbiterGetShapes(arb, &a, &b);
   chipmunkPhysics->SendCollisionPairRemoveEvent(arb, a, b);
}

void Physics2D::DestroyActorDelegate(IEventDataPtr pEventData)
{
   std::shared_ptr<EvtData_Destroy_Actor> pCastEventData = std::static_pointer_cast<EvtData_Destroy_Actor>(pEventData);
   RemoveActor(pCastEventData->GetId());
}

IGamePhysics2D* CreateGamePhysics2D()
{
   IGamePhysics2D* pGamePhysics2D = new Physics2D();
   assert(pGamePhysics2D);
   return pGamePhysics2D;
}

} // namespace BIEngine