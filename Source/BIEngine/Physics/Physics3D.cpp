#include "Physics3D.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <iterator>

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_relational.hpp>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

#include "../StdLib/Assert.h"
#include "../StdLib/HashSet.h"
#include "../StdLib/HashMap.h"
#include "../EngineCore/GameApp.h"
#include "../Actors/Actor.h"
#include "../Actors/TransformComponent.h"
#include "../EventManager/EventManager.h"
#include "../EventManager/Events.h"
#include "../Utilities/GameTimer.h"
#include "Physics3DEventListener.h"
#include "PhysicsDebugDrawer.h"

namespace BIEngine {

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

// Определение отсутсвующей физи с помощью паттерна "нулевой объект"
class NullPhysics3D : public IGamePhysics3D {
public:
   NullPhysics3D() {}

   virtual ~NullPhysics3D() {}

   virtual bool Initialize() override { return true; }

   virtual void SetGravity(const glm::vec3& gravity) override {};

   virtual void BeforeUpdate(const HashMap<ActorId, SharedPtr<Actor>>& actorMap) override {};

   virtual void OnUpdate(float dt) override {}

   virtual void AfterUpdate(const HashMap<ActorId, SharedPtr<Actor>>& actorMap) override {};

   virtual void DrawRenderDiagnostics() override {}

   virtual void AddSphere(float radius, const ShapeCreationParams& creationParams) override {}

   virtual void AddBox(const glm::vec3& dimensions, const ShapeCreationParams& creationParams) override {}

   virtual void AddCapsule(const float radius, const float height, const ShapeCreationParams& creationParams) override {}

   virtual void AddPointCloud(const glm::vec3* verts, int numPoints, const ShapeCreationParams& creationParams) override {}

   virtual void RemoveActor(ActorId id) override {}

   virtual void ApplyForce(const glm::vec3& forceVec, ActorId aid) override {}

   virtual void ApplyTorque(const glm::vec3& torque, ActorId aid) override {}

   virtual bool Translate(ActorId aid, const glm::vec3& displacement, const glm::vec3& angles) override { return true; }

   virtual RaycastInfo Raycast(const glm::vec3& from, const glm::vec3& to) override { return RaycastInfo(); }

   virtual void RotateY(ActorId actorId, float angleRadians) override {}

   virtual float GetOrientationY(ActorId actorId) const override { return 0.0; }

   virtual void StopActor(ActorId actorId) override {}

   virtual glm::vec3 GetVelocity(ActorId actorId) const override { return glm::vec3(); }

   virtual void SetVelocity(ActorId actorId, const glm::vec3& vel) override {}

   virtual glm::vec3 GetAngularVelocity(ActorId actorId) const override { return glm::vec3(0.0f); }

   virtual void SetAngularVelocity(ActorId actorId, const glm::vec3& vel) override {}

   virtual void SetPosition(const ActorId id, const glm::vec3& position) override {}

   virtual glm::vec3 GetPosition(const ActorId id) const override { return glm::vec3(); }
};

IGamePhysics3D* CreateNullPhysics3D()
{
   IGamePhysics3D* pGamePhysics3D = new NullPhysics3D();
   return pGamePhysics3D;
}

static btVector3 Vec3_to_btVector3(const glm::vec3& vec3)
{
   return btVector3(vec3.x, vec3.y, vec3.z);
}

static glm::vec3 btVector3_to_Vec3(const btVector3& btvec)
{
   return glm::vec3(btvec.x(), btvec.y(), btvec.z());
}

static btTransform Mat4x4_to_btTransform(const glm::mat4& mat)
{
   btMatrix3x3 bulletRotation;
   btVector3 bulletPosition;

   for (int row = 0; row < 3; ++row)
      for (int column = 0; column < 3; ++column)
         bulletRotation[row][column] = mat[column][row];

   for (int column = 0; column < 3; ++column)
      bulletPosition[column] = mat[3][column];

   return btTransform(bulletRotation, bulletPosition);
}

static glm::mat4 btTransform_to_Mat4x4(const btTransform& trans)
{
   glm::mat4 returnValue = glm::mat4(1.0f);

   btMatrix3x3 const& bulletRotation = trans.getBasis();
   btVector3 const& bulletPosition = trans.getOrigin();

   for (int row = 0; row < 3; ++row)
      for (int column = 0; column < 3; ++column)
         returnValue[row][column] = bulletRotation[column][row];

   for (int column = 0; column < 3; ++column)
      returnValue[3][column] = bulletPosition[column];

   return returnValue;
}

struct ActorMotionState : public btMotionState {
   glm::mat4 m_worldToPositionTransform;

   ActorMotionState(const glm::mat4& startingTransform)
      : m_worldToPositionTransform(startingTransform) {}

   // btMotionState interface:  Bullet calls these
   virtual void getWorldTransform(btTransform& worldTrans) const
   {
      worldTrans = Mat4x4_to_btTransform(m_worldToPositionTransform);
   }

   virtual void setWorldTransform(const btTransform& worldTrans)
   {
      m_worldToPositionTransform = btTransform_to_Mat4x4(worldTrans);
   }
};

static void ApplyPhysicsBodyToActor(BIEngine::Actor& actor, const ActorMotionState& actorMotionState)
{
    BIEngine::SharedPtr<BIEngine::TransformComponent> pTransformComponent = actor.GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).Lock();
    if (!pTransformComponent) {
        return;
    }

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, pTransformComponent->GetPosition());
    glm::vec3 rotationAngles = pTransformComponent->GetRotation();
    const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));

    const glm::mat4 rotationMatrix = transformZ * transformY * transformX;
    trans *= rotationMatrix;

    if (!glm::all(glm::equal(trans, actorMotionState.m_worldToPositionTransform, glm::epsilon<float>()))) {
        // We don't pass actorMotionState->m_worldToPositionTransform into Transform Component because it doesn't have a scale
        btTransform btTrans;
        actorMotionState.getWorldTransform(btTrans);
        const glm::vec3 pos = btVector3_to_Vec3(btTrans.getOrigin());
        glm::vec3 rotation;
        btQuaternion qtRot = btTrans.getRotation();

        qtRot.getEulerZYX(rotation.z, rotation.y, rotation.x);
        rotation.x = glm::degrees(rotation.x);
        rotation.y = glm::degrees(rotation.y);
        rotation.z = glm::degrees(rotation.z);
        pTransformComponent->SetPosition(pos);
        pTransformComponent->SetRotation(rotation);

        SharedPtr<EvtData_Move_Actor> pEvent = MakeShared<EvtData_Move_Actor>(actor.GetId(), pTransformComponent->GetPosition(), pTransformComponent->GetRotation());
        EventManager::Get()->QueueEvent(pEvent);
    }
}

class KinematicSweepCallback final : public btCollisionWorld::ClosestConvexResultCallback {
public:
    KinematicSweepCallback(const btCollisionObject* pMovingObject, const btVector3& from, const btVector3& to)
        : btCollisionWorld::ClosestConvexResultCallback(from, to),
        m_pMovingObject(pMovingObject),
        m_sweepDirection(to - from)
    {
    }

    virtual bool needsCollision(btBroadphaseProxy* pProxy) const override
    {
        if (!btCollisionWorld::ClosestConvexResultCallback::needsCollision(pProxy)) {
            return false;
        }

        const btCollisionObject* const pCollisionObject = static_cast<const btCollisionObject*>(pProxy->m_clientObject);
        return pCollisionObject != m_pMovingObject && pCollisionObject->hasContactResponse();
    }

    virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace) override
    {
        const btVector3 hitNormal = normalInWorldSpace
            ? convexResult.m_hitNormalLocal
            : convexResult.m_hitCollisionObject->getWorldTransform().getBasis() * convexResult.m_hitNormalLocal;

        // Ignore contacts which do not oppose the requested movement. This prevents
        // the floor under a capsule from blocking a horizontal sweep.
        if (hitNormal.dot(m_sweepDirection) >= -SIMD_EPSILON) {
            return btScalar(1.0f);
        }

        return btCollisionWorld::ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
    }

private:
    const btCollisionObject* m_pMovingObject;
    btVector3 m_sweepDirection;
};

class KinematicPenetrationCallback final : public btCollisionWorld::ContactResultCallback {
public:
    explicit KinematicPenetrationCallback(const btCollisionObject* pMovingObject)
        : m_pMovingObject(pMovingObject)
    {
    }

    virtual btScalar addSingleResult(
        btManifoldPoint& contactPoint,
        const btCollisionObjectWrapper* pObject0,
        int partId0,
        int index0,
        const btCollisionObjectWrapper* pObject1,
        int partId1,
        int index1) override
    {
        if (contactPoint.getDistance() >= 0.0f) {
            return 0.0f;
        }

        const btCollisionObject* const pCollisionObject0 = pObject0->getCollisionObject();
        const btCollisionObject* const pCollisionObject1 = pObject1->getCollisionObject();
        const btCollisionObject* const pOtherObject = pCollisionObject0 == m_pMovingObject ? pCollisionObject1 : pCollisionObject0;
        if (!pOtherObject->hasContactResponse()) {
            return 0.0f;
        }

        const btScalar penetrationDepth = -contactPoint.getDistance();
        if (penetrationDepth > m_penetrationDepth) {
            m_penetrationDepth = penetrationDepth;
            m_recoveryNormal = pCollisionObject0 == m_pMovingObject
                ? contactPoint.m_normalWorldOnB
                : -contactPoint.m_normalWorldOnB;
        }

        return 0.0f;
    }

    bool HasPenetration() const { return m_penetrationDepth > std::numeric_limits<float>::epsilon(); }
    btScalar GetPenetrationDepth() const { return m_penetrationDepth; }
    const btVector3& GetRecoveryNormal() const { return m_recoveryNormal; }

private:
    const btCollisionObject* m_pMovingObject;
    btScalar m_penetrationDepth = 0.0f;
    btVector3 m_recoveryNormal = btVector3(0.0f, 1.0f, 0.0f);
};

class Physics3D : public IGamePhysics3D {
public:
   Physics3D();
   virtual ~Physics3D();

   Physics3D(const Physics3D& orig) = delete;
   Physics3D& operator=(const Physics3D& rhs) = delete;

   // Инициализация физического движка и загузка информации о физических материалах из XML-файла
   virtual bool Initialize() override;
   // Задает двумерный вектор гравитации.
   virtual void SetGravity(const glm::vec3& gravity) override;

   virtual void BeforeUpdate(const HashMap<ActorId, SharedPtr<Actor>>& actorMap) override;
   virtual void OnUpdate(float dt) override;
   virtual void AfterUpdate(const HashMap<ActorId, SharedPtr<Actor>>& actorMap) override;

   virtual void DrawRenderDiagnostics() override;

   virtual void AddSphere(float radius, const ShapeCreationParams& creationParams) override;
   virtual void AddBox(const glm::vec3& dimensions, const ShapeCreationParams& creationParams) override;
   virtual void AddCapsule(const float radius, const float height, const ShapeCreationParams& creationParams) override;
   virtual void AddPointCloud(const glm::vec3* verts, int numPoints, const ShapeCreationParams& creationParams) override;
   virtual void RemoveActor(ActorId id) override;

   // Применяет силу к объекту
   virtual void ApplyForce(const glm::vec3& forceVec, ActorId aid) override;
   // Применить момент силы к объекту
   virtual void ApplyTorque(const glm::vec3& torque, ActorId aid) override;

   virtual RaycastInfo Raycast(const glm::vec3& from, const glm::vec3& to) override;

   bool Translate(ActorId aid, const glm::vec3& displacement, const glm::vec3& angles);
   // Напрямую задает поворот физического объекта.
   // Следует быть с этим аккуратнее, так как данная процедура способна сломать физическую симуляцию
   virtual void RotateY(ActorId actorId, float const deltaAngleRadians);
   virtual float GetOrientationY(ActorId actorId) const;
   // Приравниваем скорость актера к нулевому вектору
   virtual void StopActor(ActorId actorId) override;
   virtual glm::vec3 GetVelocity(ActorId actorId) const override;
   virtual void SetVelocity(ActorId actorId, const glm::vec3& vel) override;
   virtual glm::vec3 GetAngularVelocity(ActorId actorId) const override;
   virtual void SetAngularVelocity(ActorId actorId, const glm::vec3& vel) override;
   virtual void SetPosition(const ActorId id, const glm::vec3& position) override;
   virtual glm::vec3 GetPosition(const ActorId id) const override;

private:
   // Инициализация и работа с физическими материалами
   void LoadXml(tinyxml2::XMLElement* pRoot);
   float LookupSpecificGravity(const String& densityStr);
   MaterialData LookupMaterialData(const String& materialStr);

   btRigidBody* FindBulletRigidBody(ActorId const id) const;
   ActorId FindActorID(const btRigidBody* const body) const;

   void AddShape(btCollisionShape* const pShape, const float volume, const ShapeCreationParams& creationParams);

   // Работа с коллизией объектов
   void RemoveCollisionObject(btCollisionObject* const pBodyToRemove);
   void SendCollisionPairAddEvent(btPersistentManifold const* manifold, const btRigidBody* const body0, const btRigidBody* const body1);
   void SendCollisionPairRemoveEvent(const btRigidBody* const body0, const btRigidBody* const body1);
   static void BulletInternalTickCallback(btDynamicsWorld* const world, const btScalar timeStep);

   // Удаление уничтоженного актера из симуляции
   void DestroyActorDelegate(IEventDataPtr pEventData);

private:
   EventManager::DelegateHandler m_destroyActorDelegateHandler;

   // Главная точка управления в bullet
   btDynamicsWorld* m_pDynamicsWorld;
   // Bullet сначала ищет коллизии возможное коллизии между объектами с помощью Broadphase, а потом отправляет всех подозреваемых в CollisionDispatcher на дополнительный обсчет
   btBroadphaseInterface* m_pBroadphase;
   btCollisionDispatcher* m_pDispatcher;
   btConstraintSolver* m_pSolver;
   // Управляет внутренний памятью во время тика менеджера коллизии
   btDefaultCollisionConfiguration* m_pCollisionConfiguration;

   BulletDebugDrawer* m_pDebugDrawer;
   bool m_isRenderDebugDiagnostics = false;

   using DensityTable = HashMap<String, float>;
   using MaterialTable = HashMap<String, MaterialData>;
   DensityTable m_densityTable;
   MaterialTable m_materialTable;

   using ActorIDToBulletRigidBodyMap = HashMap<ActorId, btRigidBody*>;
   ActorIDToBulletRigidBodyMap m_actorIdToRigidBody;

   using BulletRigidBodyToActorIDMap = HashMap<btRigidBody const*, ActorId>;
   BulletRigidBodyToActorIDMap m_rigidBodyToActorId;

   using CollisionPair = Pair<btRigidBody const*, btRigidBody const*>;

   class HashCollisionPair {
   public:
      SizeT operator()(const CollisionPair& pair)
      {
         SizeT hash = 0;
         HashCombine(hash, pair.first);
         HashCombine(hash, pair.second);
         return hash;
      }
   };

   using CollisionPairs = HashSet<CollisionPair, HashCollisionPair>;
   CollisionPairs m_previousTickCollisionPairs;
};

Physics3D::Physics3D()
   : IGamePhysics3D()
{
   m_destroyActorDelegateHandler = EventManager::Get()->AddListener(MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(Physics3D::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);
}

Physics3D::~Physics3D()
{
   EventManager::Get()->RemoveListener(m_destroyActorDelegateHandler);

   for (int ii = m_pDynamicsWorld->getNumCollisionObjects() - 1; ii >= 0; --ii) {
      btCollisionObject* const obj = m_pDynamicsWorld->getCollisionObjectArray()[ii];

      RemoveCollisionObject(obj);
   }

   m_rigidBodyToActorId.Clear();

   if (m_pDynamicsWorld) {
      delete m_pDynamicsWorld;
      m_pDynamicsWorld = nullptr;
   }

   if (m_pSolver) {
      delete m_pSolver;
      m_pSolver = nullptr;
   }

   if (m_pBroadphase) {
      delete m_pBroadphase;
      m_pBroadphase = nullptr;
   }

   if (m_pDispatcher) {
      delete m_pDispatcher;
      m_pDispatcher = nullptr;
   }

   if (m_pCollisionConfiguration) {
      delete m_pCollisionConfiguration;
      m_pCollisionConfiguration = nullptr;
   }

   if (m_pDebugDrawer) {
      delete m_pDebugDrawer;
      m_pDebugDrawer = nullptr;
   }
}

bool Physics3D::Initialize()
{
   auto xmlExtraData = StaticPointerCast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle("config/physics.xml")->GetExtra());
   LoadXml(xmlExtraData->GetRootElement());

   m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
   m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);
   m_pBroadphase = new btDbvtBroadphase();
   m_pSolver = new btSequentialImpulseConstraintSolver();

   m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);

   m_pDebugDrawer = new BulletDebugDrawer();
   m_pDebugDrawer->ReadOptions();

   if (!m_pCollisionConfiguration || !m_pDispatcher || !m_pBroadphase || !m_pSolver || !m_pDynamicsWorld || !m_pDebugDrawer) {
      Logger::WriteLog(Logger::LogType::ERROR, "BulletPhysics::VInitialize failed!");
      return false;
   }

   m_pDynamicsWorld->setDebugDrawer(m_pDebugDrawer);

   m_pDynamicsWorld->setInternalTickCallback(BulletInternalTickCallback);
   m_pDynamicsWorld->setWorldUserInfo(this);

   return true;
}

void Physics3D::SetGravity(const glm::vec3& gravity)
{
   m_pDynamicsWorld->setGravity(Vec3_to_btVector3(gravity));
}

void Physics3D::BeforeUpdate(const HashMap<ActorId, SharedPtr<Actor>>& actorMap)
{
   for (auto it = m_actorIdToRigidBody.CBegin(); it != m_actorIdToRigidBody.CEnd(); ++it) {
      const ActorId id = it->first;

      ActorMotionState* const actorMotionState = static_cast<ActorMotionState*>(it->second->getMotionState());
      Assert(actorMotionState, "Rigid body doesn't nave motion state");

      if (!actorMotionState) {
         continue;
      }

      auto actorIt = actorMap.Find(id);
      Assert(actorIt != actorMap.CEnd(), "Actor with id %d registered in physics system but doesn't exist", id);
      if (actorIt == actorMap.CEnd()) {
         continue;
      }

      SharedPtr<Actor> pGameActor = actorIt->second;
      if (pGameActor && actorMotionState) {
         SharedPtr<TransformComponent> pTransformComponent = pGameActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).Lock();
         if (pTransformComponent) {
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, pTransformComponent->GetPosition());
            glm::vec3 rotationAngles = pTransformComponent->GetRotation();
            const glm::quat quat = glm::quat(glm::vec3(glm::radians(rotationAngles.x), glm::radians(rotationAngles.y), glm::radians(rotationAngles.z)));

            trans *= glm::mat4_cast(quat);

            actorMotionState->setWorldTransform(Mat4x4_to_btTransform(trans));
            m_actorIdToRigidBody[pGameActor->GetId()]->setWorldTransform(Mat4x4_to_btTransform(trans));
         }
      }
   }
}

void Physics3D::OnUpdate(float dt)
{
   m_pDynamicsWorld->stepSimulation(dt, 0);
}

void Physics3D::AfterUpdate(const HashMap<ActorId, SharedPtr<Actor>>& actorMap)
{
   for (auto it = m_actorIdToRigidBody.CBegin(); it != m_actorIdToRigidBody.CEnd(); ++it) {
      const ActorId id = it->first;

      const ActorMotionState* const actorMotionState = static_cast<ActorMotionState*>(it->second->getMotionState());
      Assert(actorMotionState, "Rigid body doesn't nave motion state");

      if (!actorMotionState) {
         continue;
      }

      auto actorIt = actorMap.Find(id);
      Assert(actorIt != actorMap.CEnd(), "Actor with id %d registered in physics system but doesn't exist", id);
      if (actorIt == actorMap.CEnd()) {
         continue;
      }

      SharedPtr<Actor> pGameActor = actorIt->second;
      if (pGameActor && actorMotionState) {
          ApplyPhysicsBodyToActor(*pGameActor, *actorMotionState);
      }
   }
}

void Physics3D::DrawRenderDiagnostics()
{
   ImGui::SetNextWindowSize(ImVec2(275, 100), ImGuiCond_Always);

   if (!ImGui::Begin("Physcis3D settings")) {
      ImGui::End();
      return;
   }

   ImGui::Text("Debug");
   ImGui::Checkbox("Render Debug Diagnostics", &m_isRenderDebugDiagnostics);

   ImGui::End();

   if (m_isRenderDebugDiagnostics) {
      m_pDynamicsWorld->debugDrawWorld();
   }
}

void Physics3D::AddShape(btCollisionShape* const pShape, const float volume, const ShapeCreationParams& creationParams)
{
   Assert(m_actorIdToRigidBody.Find(creationParams.actorId) == m_actorIdToRigidBody.End(), "Actor with more than one physics body?");

   const float specificGravity = LookupSpecificGravity(creationParams.densityStr);
   const btScalar mass = creationParams.bodyType == BodyType::DYNAMIC ? volume * specificGravity : 0.0f;

   MaterialData material(LookupMaterialData(creationParams.physicsMaterial));

   btVector3 localInertia(0.0f, 0.0f, 0.0f);
   if (mass > 0.0f) {
      pShape->calculateLocalInertia(mass, localInertia);
   }

   glm::mat4 transform = glm::mat4(1.0f);
   transform = glm::translate(transform, creationParams.pos);
   transform = glm::rotate(transform, glm::radians(creationParams.eulerAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
   transform = glm::rotate(transform, glm::radians(creationParams.eulerAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
   transform = glm::rotate(transform, glm::radians(creationParams.eulerAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));

   ActorMotionState* const myMotionState = new ActorMotionState(transform);

   btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, pShape, localInertia);

   rbInfo.m_restitution = material.m_restitution;
   rbInfo.m_friction = material.m_friction;

   btRigidBody* const body = new btRigidBody(rbInfo);
   body->setAngularFactor(Vec3_to_btVector3(creationParams.angularFactor));

   if (creationParams.bodyType == BodyType::KINEMATIC) {
       body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
       body->setActivationState(DISABLE_DEACTIVATION);
   }

   m_pDynamicsWorld->addRigidBody(body);

   if (creationParams.isTrigger) {
      body->setCollisionFlags(body->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE);
      body->setUserPointer((void*)creationParams.actorId);
   }

   m_actorIdToRigidBody[creationParams.actorId] = body;
   m_rigidBodyToActorId[body] = creationParams.actorId;
}

void Physics3D::AddSphere(float radius, const ShapeCreationParams& creationParams)
{
   btSphereShape* const collisionShape = new btSphereShape(radius);

   // calculate absolute mass from specificGravity
   float const volume = (4.f / 3.f) * M_PI * radius * radius * radius;

   AddShape(collisionShape, volume, creationParams);
}

void Physics3D::AddBox(const glm::vec3& dimensions, const ShapeCreationParams& creationParams)
{
   glm::vec3 dimensionsHalfExtents = dimensions;
   dimensionsHalfExtents.x /= 2;
   dimensionsHalfExtents.y /= 2;
   dimensionsHalfExtents.z /= 2;

   btBoxShape* const collisionShape = new btBoxShape(Vec3_to_btVector3(dimensionsHalfExtents));

   float const volume = dimensions.x * dimensions.y * dimensions.z;

   AddShape(collisionShape, volume, creationParams);
}

void Physics3D::AddCapsule(const float radius, const float height, const ShapeCreationParams& creationParams)
{
   btCapsuleShape* const collisionShape = new btCapsuleShape(radius, height);

   const float volume = ((4.0f / 3.0f) * radius + height) * M_PI * radius * radius;

   AddShape(collisionShape, volume, creationParams);
}

void Physics3D::AddPointCloud(const glm::vec3* verts, int numPoints, const ShapeCreationParams& creationParams)
{
   btConvexHullShape* const collisionShape = new btConvexHullShape();

   for (int ii = 0; ii < numPoints; ++ii) {
      collisionShape->addPoint(Vec3_to_btVector3(verts[ii]));
   }

   btVector3 aabbMin(0, 0, 0), aabbMax(0, 0, 0);
   collisionShape->getAabb(btTransform::getIdentity(), aabbMin, aabbMax);

   btVector3 const aabbExtents = aabbMax - aabbMin;
   float const volume = aabbExtents.x() * aabbExtents.y() * aabbExtents.z();

   AddShape(collisionShape, volume, creationParams);
}

void Physics3D::RemoveActor(ActorId id)
{
   if (btRigidBody* const body = FindBulletRigidBody(id)) {
      RemoveCollisionObject(body);
      m_actorIdToRigidBody.Erase(id);
      m_rigidBodyToActorId.Erase(body);
   }
}

void Physics3D::ApplyForce(const glm::vec3& forceVec, ActorId aid)
{
   if (btRigidBody* const body = FindBulletRigidBody(aid)) {
      body->setActivationState(ACTIVE_TAG);
      body->applyCentralImpulse(Vec3_to_btVector3(forceVec));
   }
}

void Physics3D::ApplyTorque(const glm::vec3& torque, ActorId aid)
{
   if (btRigidBody* const body = FindBulletRigidBody(aid)) {
      body->setActivationState(ACTIVE_TAG);
      body->applyTorqueImpulse(Vec3_to_btVector3(torque));
   }
}

bool Physics3D::Translate(ActorId aid, const glm::vec3& displacement, const glm::vec3& angles)
{
    constexpr float RECOVERY_OFFSET = 0.02;

    if (btRigidBody* const body = FindBulletRigidBody(aid)) {
        btCollisionShape* const collisionShape = body->getCollisionShape();
        Assert(collisionShape && collisionShape->isConvex(), "A kinematic character requires a convex collision shape");
        if (!collisionShape || !collisionShape->isConvex()) {
            return false;
        }

        glm::mat4 rotationTransform = glm::mat4(1.0f);
        rotationTransform = glm::rotate(rotationTransform, glm::radians(angles.z), glm::vec3(0.0f, 0.0f, 1.0f));
        rotationTransform = glm::rotate(rotationTransform, glm::radians(angles.y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotationTransform = glm::rotate(rotationTransform, glm::radians(angles.x), glm::vec3(1.0f, 0.0f, 0.0f));

        btTransform resolvedTransform = body->getWorldTransform();
        resolvedTransform.setBasis(Mat4x4_to_btTransform(rotationTransform).getBasis());

        auto applyTransform = [&](const btTransform& transform) {
            body->setWorldTransform(transform);
            body->setInterpolationWorldTransform(transform);

            ActorMotionState* const motionState = static_cast<ActorMotionState*>(body->getMotionState());
            if (motionState) {
                motionState->setWorldTransform(transform);
            }

            m_pDynamicsWorld->updateSingleAabb(body);
        };

        auto configureCallback = [&](auto& callback) {
            callback.m_collisionFilterGroup = btBroadphaseProxy::DefaultFilter;
            callback.m_collisionFilterMask = btBroadphaseProxy::AllFilter;
        };

        btConvexShape* const convexShape = static_cast<btConvexShape*>(collisionShape);

        constexpr int MAX_PENETRATION_RECOVERY_ITERATIONS = 4;
        for (int i = 0; i < MAX_PENETRATION_RECOVERY_ITERATIONS; ++i) {
            applyTransform(resolvedTransform);

            KinematicPenetrationCallback penetrationCallback(body);
            configureCallback(penetrationCallback);
            m_pDynamicsWorld->contactTest(body, penetrationCallback);
            if (!penetrationCallback.HasPenetration()) {
                break;
            }

            btVector3 recoveryNormal = penetrationCallback.GetRecoveryNormal();
            if (recoveryNormal.length2() <= std::numeric_limits<float>::epsilon()) {
                break;
            }

            recoveryNormal.normalize();
            resolvedTransform.getOrigin() += recoveryNormal * (penetrationCallback.GetPenetrationDepth() + RECOVERY_OFFSET);
        }

        btVector3 remainingDisplacement = Vec3_to_btVector3(displacement);
        constexpr int MAX_SWEEP_ITERATIONS = 4;
        for (int i = 0; i < MAX_SWEEP_ITERATIONS && remainingDisplacement.length2() > std::numeric_limits<float>::epsilon(); ++i) {
            btTransform targetTransform = resolvedTransform;
            targetTransform.getOrigin() += remainingDisplacement;

            KinematicSweepCallback sweepCallback(body, resolvedTransform.getOrigin(), targetTransform.getOrigin());
            configureCallback(sweepCallback);
            m_pDynamicsWorld->convexSweepTest(convexShape, resolvedTransform, targetTransform, sweepCallback);

            if (!sweepCallback.hasHit()) {
                resolvedTransform = targetTransform;
                break;
            }

            btVector3 hitNormal = sweepCallback.m_hitNormalWorld;
            hitNormal.normalize();

            const btScalar movementLength = remainingDisplacement.length();
            const btScalar skinFraction = movementLength > std::numeric_limits<float>::epsilon() ? RECOVERY_OFFSET / movementLength : 0.0f;
            const btScalar travelFraction = btMax(btScalar(0.0f), sweepCallback.m_closestHitFraction - skinFraction);
            resolvedTransform.getOrigin() += remainingDisplacement * travelFraction;

            btVector3 unresolvedDisplacement = remainingDisplacement * (btScalar(1.0f) - travelFraction);
            btVector3 slideNormal = hitNormal;

            const btScalar movementIntoSurface = unresolvedDisplacement.dot(slideNormal);
            if (movementIntoSurface < 0.0f) {
                unresolvedDisplacement -= slideNormal * movementIntoSurface;
            }

            remainingDisplacement = unresolvedDisplacement;
        }

        applyTransform(resolvedTransform);

        const ActorMotionState* const motionState = static_cast<ActorMotionState*>(body->getMotionState());
        
        BIEngine::SharedPtr<BIEngine::Actor> pActor =  g_pApp->m_pGameLogic->GetActor(aid);
        ApplyPhysicsBodyToActor(*pActor, *motionState);

        return true;
    }

    return false;
}

IGamePhysics3D::RaycastInfo Physics3D::Raycast(const glm::vec3& from, const glm::vec3& to)
{
   const btVector3 btFrom = Vec3_to_btVector3(from);
   const btVector3 btTo = Vec3_to_btVector3(to);

   btCollisionWorld::ClosestRayResultCallback closestResults(btFrom, btTo);
   closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

   m_pDynamicsWorld->rayTest(btFrom, btTo, closestResults);

   IGamePhysics3D::RaycastInfo info;

   if (closestResults.hasHit()) {
      info.hasHit = true;

      btVector3 p = btFrom.lerp(btTo, closestResults.m_closestHitFraction);
      info.hitPosition = btVector3_to_Vec3(p);

      if (closestResults.m_collisionObject) {
         const btRigidBody* const body = btRigidBody::upcast(closestResults.m_collisionObject);
         info.hitActor = g_pApp->m_pGameLogic->GetActor(FindActorID(body));
      }
   }

   return info;
}

void Physics3D::RotateY(ActorId const actorId, float const deltaAngleRadians)
{
   btRigidBody* pRigidBody = FindBulletRigidBody(actorId);
   Assert(pRigidBody, "There is not rigid body for this actor. Add it first");

   if (pRigidBody) {
      btTransform angleTransform;
      angleTransform.setIdentity();
      angleTransform.getBasis().setEulerYPR(0, deltaAngleRadians, 0);

      pRigidBody->setCenterOfMassTransform(pRigidBody->getCenterOfMassTransform() * angleTransform);
   }
}

float Physics3D::GetOrientationY(ActorId actorId) const
{
   btRigidBody* pRigidBody = FindBulletRigidBody(actorId);
   Assert(pRigidBody, "There is not rigid body for this actor. Add it first");

   if (!pRigidBody) {
      return std::numeric_limits<float>::max();
   }

   const btTransform& actorTransform = pRigidBody->getCenterOfMassTransform();
   btMatrix3x3 actorRotationMat(actorTransform.getBasis());

   btVector3 startingVec(0, 0, 1);
   btVector3 endingVec = actorRotationMat * startingVec;

   endingVec.setY(0);

   float const endingVecLength = endingVec.length();
   if (endingVecLength < 0.001) {
      // Складывание рамок
      return 0;
   } else {
      btVector3 cross = startingVec.cross(endingVec);
      float sign = cross.getY() > 0 ? 1.0f : -1.0f;
      return (acosf(startingVec.dot(endingVec) / endingVecLength) * sign);
   }

   return std::numeric_limits<float>::max();
}

void Physics3D::StopActor(ActorId actorId)
{
   SetVelocity(actorId, glm::vec3(0.f, 0.f, 0.f));
}

glm::vec3 Physics3D::GetVelocity(ActorId actorId) const
{
   btRigidBody* pRigidBody = FindBulletRigidBody(actorId);
   Assert(pRigidBody, "There is not rigid body for this actor. Add it first");
   if (!pRigidBody) {
      return glm::vec3(0.0f);
   }

   btVector3 btVel = pRigidBody->getLinearVelocity();
   return btVector3_to_Vec3(btVel);
}

void Physics3D::SetVelocity(ActorId actorId, const glm::vec3& vel)
{
   btRigidBody* pRigidBody = FindBulletRigidBody(actorId);
   Assert(pRigidBody, "There is not rigid body for this actor. Add it first");
   if (!pRigidBody) {
      return;
   }

   pRigidBody->setActivationState(ACTIVE_TAG);

   btVector3 btVel = Vec3_to_btVector3(vel);
   pRigidBody->setLinearVelocity(btVel);
}

glm::vec3 Physics3D::GetAngularVelocity(ActorId actorId) const
{
   btRigidBody* pRigidBody = FindBulletRigidBody(actorId);
   Assert(pRigidBody, "There is not rigid body for this actor. Add it first");

   if (!pRigidBody)
      return glm::vec3(0.0f);

   btVector3 btVel = pRigidBody->getAngularVelocity();
   return btVector3_to_Vec3(btVel);
}

void Physics3D::SetAngularVelocity(ActorId actorId, const glm::vec3& vel)
{
   btRigidBody* pRigidBody = FindBulletRigidBody(actorId);
   Assert(pRigidBody, "There is not rigid body for this actor. Add it first");

   if (!pRigidBody)
      return;

   btVector3 btVel = Vec3_to_btVector3(vel);
   pRigidBody->setAngularVelocity(btVel);
}

void Physics3D::SetPosition(const ActorId id, const glm::vec3& position)
{
   btRigidBody* pRigidBody = FindBulletRigidBody(id);
   Assert(pRigidBody, "There is not rigid body for this actor. Add it first");

   if (pRigidBody) {
      btVector3 btVec = Vec3_to_btVector3(position);

      btTransform& transform = pRigidBody->getWorldTransform();
      transform.setOrigin(btVec);

      pRigidBody->setWorldTransform(transform);
   }
}

glm::vec3 Physics3D::GetPosition(const ActorId id) const
{
   btRigidBody* pRigidBody = FindBulletRigidBody(id);
   Assert(pRigidBody, "There is not rigid body for this actor. Add it first");

   if (pRigidBody) {
      btVector3 pos = pRigidBody->getWorldTransform().getOrigin();
      return btVector3_to_Vec3(pos);
   }

   return glm::vec3(0.0f);
}

void Physics3D::LoadXml(tinyxml2::XMLElement* pRoot)
{
   Assert(pRoot, "Provided bad arguments");

   tinyxml2::XMLElement* pParentNode = pRoot->FirstChildElement("PhysicsMaterials");
   Assert(pParentNode, "Physics3D settings must have PhysicsMaterials element!");
   if (!pParentNode) {
      return;
   }

   for (tinyxml2::XMLElement* pNode = pParentNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
      double restitution = 0;
      double friction = 0;
      pNode->QueryDoubleAttribute("restitution", &restitution);
      pNode->QueryDoubleAttribute("friction", &friction);
      m_materialTable.Insert(pNode->Value(), MaterialData((float)restitution, (float)friction));
   }

   pParentNode = pRoot->FirstChildElement("DensityTable");
   Assert(pParentNode, "Physics3D settings must have DensityTable element!");
   if (!pParentNode) {
      return;
   }
   for (tinyxml2::XMLElement* pNode = pParentNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
      m_densityTable.Insert(pNode->Value(), (float)atof(pNode->FirstChild()->Value()));
   }
}

float Physics3D::LookupSpecificGravity(const String& densityStr)
{
   float density = 0;
   auto densityIt = m_densityTable.Find(densityStr);
   if (densityIt != m_densityTable.End())
      density = densityIt->second;

   return density;
}

MaterialData Physics3D::LookupMaterialData(const String& materialStr)
{
   auto materialIt = m_materialTable.Find(materialStr);
   if (materialIt != m_materialTable.End())
      return materialIt->second;
   else
      return MaterialData(0.0, 0.0);
}

btRigidBody* Physics3D::FindBulletRigidBody(ActorId const id) const
{
   auto foundItr = m_actorIdToRigidBody.Find(id);
   if (foundItr != m_actorIdToRigidBody.CEnd()) {
      return foundItr->second;
   }

   return nullptr;
}

ActorId Physics3D::FindActorID(const btRigidBody* const pBody) const
{
   auto foundItr = m_rigidBodyToActorId.Find(pBody);
   if (foundItr != m_rigidBodyToActorId.CEnd()) {
      return foundItr->second;
   }

   return Actor::INVALID_ACTOR_ID;
}

void Physics3D::RemoveCollisionObject(btCollisionObject* const pBodyToRemove)
{
   // Сначала удаляем объект из симуляции
   m_pDynamicsWorld->removeCollisionObject(pBodyToRemove);

   // Потом удаляем объект из списка столкновений
   for (auto it = m_previousTickCollisionPairs.Begin(); it != m_previousTickCollisionPairs.End();) {
      auto next = it;
      ++next;

      if (it->first == pBodyToRemove || it->second == pBodyToRemove) {
         SendCollisionPairRemoveEvent(it->first, it->second);
         m_previousTickCollisionPairs.Erase(it);
      }

      it = next;
   }

   if (btRigidBody* const body = btRigidBody::upcast(pBodyToRemove)) {
      // delete the components of the object
      delete body->getMotionState();
      delete body->getCollisionShape();

      for (int ii = body->getNumConstraintRefs() - 1; ii >= 0; --ii) {
         btTypedConstraint* const constraint = body->getConstraintRef(ii);
         m_pDynamicsWorld->removeConstraint(constraint);
         delete constraint;
      }
   }

   delete pBodyToRemove;
}

void Physics3D::SendCollisionPairAddEvent(btPersistentManifold const* manifold, const btRigidBody* const body0, const btRigidBody* const body1)
{
   if (body0->getUserPointer() || body1->getUserPointer()) {
      // Только триггер имеет ненулевой userPointer. Найдем кто из пришедших тех им является
      const btRigidBody *triggerBody, *otherBody;

      if (body0->getUserPointer()) {
         triggerBody = body0;
         otherBody = body1;
      } else {
         otherBody = body0;
         triggerBody = body1;
      }

      const ActorId triggerId = (ActorId)triggerBody->getUserPointer();
      SharedPtr<EvtData_Phys3DTrigger_Enter> pEvent = MakeShared<EvtData_Phys3DTrigger_Enter>(triggerId, FindActorID(otherBody));
      EventManager::Get()->QueueEvent(pEvent);
   } else {
      ActorId const id0 = FindActorID(body0);
      ActorId const id1 = FindActorID(body1);

      if (id0 == Actor::INVALID_ACTOR_ID || id1 == Actor::INVALID_ACTOR_ID) {
         return;
      }

      // this pair of colliding objects is new.  send a collision-begun event
      DynamicArray<glm::vec3> collisionPoints;
      glm::vec3 sumNormalForce;
      glm::vec3 sumFrictionForce;

      for (int pointIdx = 0; pointIdx < manifold->getNumContacts(); ++pointIdx) {
         btManifoldPoint const& point = manifold->getContactPoint(pointIdx);

         collisionPoints.PushBack(btVector3_to_Vec3(point.getPositionWorldOnB()));

         sumNormalForce += btVector3_to_Vec3(point.m_combinedRestitution * point.m_normalWorldOnB);
         sumFrictionForce += btVector3_to_Vec3(point.m_combinedFriction * point.m_lateralFrictionDir1);
      }

      SharedPtr<EvtData_Phys3DCollision> pEvent = MakeShared<EvtData_Phys3DCollision>(id0, id1, sumNormalForce, sumFrictionForce.length(), collisionPoints);
      EventManager::Get()->QueueEvent(pEvent);
   }
}

void Physics3D::SendCollisionPairRemoveEvent(const btRigidBody* const body0, const btRigidBody* const body1)
{
   if (body0->getUserPointer() || body1->getUserPointer()) {
      // Только триггер имеет ненулевой userPointer. Найдем кто из пришедших тех им является
      const btRigidBody *triggerBody, *otherBody;

      if (body0->getUserPointer()) {
         triggerBody = body0;
         otherBody = body1;
      } else {
         otherBody = body0;
         triggerBody = body1;
      }

      const ActorId triggerId = (ActorId)triggerBody->getUserPointer();
      SharedPtr<EvtData_Phys3DTrigger_Leave> pEvent = MakeShared<EvtData_Phys3DTrigger_Leave>(triggerId, FindActorID(otherBody));
      EventManager::Get()->QueueEvent(pEvent);
   } else {
      ActorId const id0 = FindActorID(body0);
      ActorId const id1 = FindActorID(body1);

      if (id0 == Actor::INVALID_ACTOR_ID || id1 == Actor::INVALID_ACTOR_ID) {
         return;
      }

      SharedPtr<EvtData_Phys3DSeparation> pEvent = MakeShared<EvtData_Phys3DSeparation>(id0, id1);
      EventManager::Get()->QueueEvent(pEvent);
   }
}

void Physics3D::BulletInternalTickCallback(btDynamicsWorld* const world, const btScalar timeStep)
{
   Assert(world, "Provided bad arguments");
   if (!world) {
      return;
   }

   void* const pWorldUserInfo = world->getWorldUserInfo();
   Assert(pWorldUserInfo, "Physics world user info is lost");
   Physics3D* const bulletPhysics = static_cast<Physics3D*>(pWorldUserInfo);

   CollisionPairs currentTickCollisionPairs;

   // Ищем столкновения
   btDispatcher* const dispatcher = world->getDispatcher();
   for (int manifoldIdx = 0; manifoldIdx < dispatcher->getNumManifolds(); ++manifoldIdx) {

      // Получаем данные, относящиеся к столкновению между двумя столкновения (вот, что значит manifold)
      const btPersistentManifold* const manifold = dispatcher->getManifoldByIndexInternal(manifoldIdx);
      Assert(manifold, "There is not data inside bullet for manifold with idx %d (Something really bad happened)", manifoldIdx);

      if (!manifold) {
         continue;
      }

      const btRigidBody* const body0 = static_cast<const btRigidBody*>(manifold->getBody0());
      const btRigidBody* const body1 = static_cast<const btRigidBody*>(manifold->getBody1());

      // Всегда создаем пары в предсказуемом порядке
      bool const swapped = body0 > body1;

      const btRigidBody* const sortedBodyA = swapped ? body1 : body0;
      const btRigidBody* const sortedBodyB = swapped ? body0 : body1;

      CollisionPair const thisPair = MakePair(sortedBodyA, sortedBodyB);
      currentTickCollisionPairs.Insert(thisPair);

      if (bulletPhysics->m_previousTickCollisionPairs.Find(thisPair) == bulletPhysics->m_previousTickCollisionPairs.End()) {
         // Это новый контакт - отправляем событие
         bulletPhysics->SendCollisionPairAddEvent(manifold, body0, body1);
      }
   }

   for (const auto collPair : bulletPhysics->m_previousTickCollisionPairs) {
      if (currentTickCollisionPairs.Find(collPair) == currentTickCollisionPairs.End()) {
         bulletPhysics->SendCollisionPairRemoveEvent(collPair.first, collPair.second);
      }
   }

   // the current tick becomes the previous tick.  this is the way of all things.
   bulletPhysics->m_previousTickCollisionPairs = std::move(currentTickCollisionPairs);
}

void Physics3D::DestroyActorDelegate(IEventDataPtr pEventData)
{
   SharedPtr<EvtData_Destroy_Actor> pCastEventData = StaticPointerCast<EvtData_Destroy_Actor>(pEventData);
   RemoveActor(pCastEventData->GetId());
}

IGamePhysics3D* CreateGamePhysics3D()
{
   IGamePhysics3D* pGamePhysics3D = new Physics3D();
   return pGamePhysics3D;
}

} // namespace BIEngine
