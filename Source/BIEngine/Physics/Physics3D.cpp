﻿#include "Physics3D.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <set>
#include <iterator>
#include <vector>
#include <map>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_relational.hpp>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

#include "../EngineCore/GameApp.h"
#include "../Actors/Actor.h"
#include "../Actors/TransformComponent.h"
#include "../EventManager/EventManager.h"
#include "../EventManager/Events.h"
#include "Physics3DEventListener.h"
#include "PhysicsDebugDrawer.h"

namespace BIEngine
{

	//Данные о материале, из которого "изготовлен объект". Загружается из компонента актера, отвечающего за физику
	struct MaterialData
	{
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

	//Определение отсутсвующей физи с помощью паттерна "нулевой объект"
	class NullPhysics3D : public IGamePhysics3D
	{
	public:
		NullPhysics3D() { }
		virtual ~NullPhysics3D() { }

		virtual bool Initialize() override { return true; }
		virtual void SetGravity(const glm::vec3& gravity) override {};
		virtual void SyncVisibleScene(const std::map<ActorId, std::shared_ptr<Actor>>& actorMap) override { };
		virtual void OnUpdate(double) override { }
		virtual void DrawRenderDiagnostics() override {}

		virtual void AddSphere(float radius, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) override { }
		virtual void AddBox(const glm::vec3& dimensions, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) override { }
		virtual void AddPointCloud(const glm::vec3* verts, int numPoints, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) override { }
		virtual void RemoveActor(ActorId id) override { }

		virtual void CreateTrigger(std::weak_ptr<Actor> pGameActor, const glm::vec3& dim) override { }
		virtual void ApplyForce(const glm::vec3& forceVec, ActorId aid) override { }
		virtual void ApplyTorque(const glm::vec3& torque, ActorId aid) override { }
		virtual bool KinematicMove(ActorId aid, const glm::vec3& position, const glm::vec3& angles) override { return true; }

		virtual RaycastInfo Raycast(const glm::vec3& from, const glm::vec3& to) override { return RaycastInfo();  }

		virtual void RotateY(ActorId actorId, float angleRadians) override { }
		virtual float GetOrientationY(ActorId actorId) const override { return 0.0; }
		virtual void StopActor(ActorId actorId) override { }
		virtual glm::vec3 GetVelocity(ActorId actorId) const override { return glm::vec3(); }
		virtual void SetVelocity(ActorId actorId, const glm::vec3& vel) override { }
		virtual glm::vec3 GetAngularVelocity(ActorId actorId) const override { return glm::vec3(0.0f); }
		virtual void SetAngularVelocity(ActorId actorId, const glm::vec3& vel) override { }
		virtual void SetPosition(const ActorId id, const glm::vec3& position) override {}
		virtual glm::vec3 GetPosition(const ActorId id) const override { return glm::vec3(); }
	};

	IGamePhysics3D* CreateNullPhysics3D()
	{
		IGamePhysics3D* pGamePhysics3D = new NullPhysics3D();
		assert(pGamePhysics3D);
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

	struct ActorMotionState : public btMotionState
	{
		glm::mat4 m_worldToPositionTransform;

		ActorMotionState(const glm::mat4& startingTransform)
			: m_worldToPositionTransform(startingTransform) { }

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

	class Physics3D : public IGamePhysics3D
	{
	public:
		Physics3D();
		virtual ~Physics3D();

		Physics3D(const Physics3D& orig) = delete;
		Physics3D& operator=(const Physics3D& rhs) = delete;

		//Инициализация физического движка и загузка информации о физических материалах из XML-файла
		virtual bool Initialize() override;
		//Задает двумерный вектор гравитации. 
		virtual void SetGravity(const glm::vec3& gravity) override;
		//Сравнивает сохраненное местоположение актеров и то местоположение, которое хранится внутри физической симуляции.
		//В случае различия, местоположение актера обновляется
		virtual void SyncVisibleScene(const std::map<ActorId, std::shared_ptr<Actor>>& actorMap) override;
		//Шаг симуляции
		virtual void OnUpdate(double dt) override;

		virtual void DrawRenderDiagnostics() override;

		virtual void AddSphere(float radius, BodyType bodyType, std::weak_ptr<Actor> pGameActor, const std::string& densityStr, const std::string& physicsMaterial) override;
		virtual void AddBox(const glm::vec3& dimensions, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) override;
		virtual void AddPointCloud(const glm::vec3* verts, int numPoints, BodyType bodyType, std::weak_ptr<Actor> gameActor, const std::string& densityStr, const std::string& physicsMaterial) override;
		virtual void RemoveActor(ActorId id) override;

		//Добавляет триггер-объект с нулевой физикой в симуляцию
		virtual void CreateTrigger(std::weak_ptr<Actor> pGameActor, const glm::vec3& dim) override;
		//Применяет силу к объекту
		virtual void ApplyForce(const glm::vec3& forceVec, ActorId aid) override;
		//Применить момент силы к объекту
		virtual void ApplyTorque(const glm::vec3& torque, ActorId aid) override;

		virtual RaycastInfo Raycast(const glm::vec3& from, const glm::vec3& to) override;

		//Напрямую задает положение и поворот физического объекта.
		//Следует быть с этим аккуратнее, так как данная процедура способна сломать физическую симуляцию
		bool KinematicMove(ActorId aid, const glm::vec3& position, const glm::vec3& angles);
		//Напрямую задает поворот физического объекта.
		//Следует быть с этим аккуратнее, так как данная процедура способна сломать физическую симуляцию
		virtual void RotateY(ActorId actorId, float const deltaAngleRadians);
		virtual float GetOrientationY(ActorId actorId) const;
		//Приравниваем скорость актера к нулевому вектору
		virtual void StopActor(ActorId actorId) override;
		virtual glm::vec3 GetVelocity(ActorId actorId) const override;
		virtual void SetVelocity(ActorId actorId, const glm::vec3& vel) override;
		virtual glm::vec3 GetAngularVelocity(ActorId actorId) const override;
		virtual void SetAngularVelocity(ActorId actorId, const glm::vec3& vel) override;
		virtual void SetPosition(const ActorId id, const glm::vec3& position) override;
		virtual glm::vec3 GetPosition(const ActorId id) const override;

	private:

		//Инициализация и работа с физическими материалами
		void LoadXml(tinyxml2::XMLElement* pRoot);
		float LookupSpecificGravity(const std::string& densityStr);
		MaterialData LookupMaterialData(const std::string& materialStr);

		btRigidBody* FindBulletRigidBody(ActorId const id) const;
		ActorId FindActorID(const btRigidBody* const body) const;

		void AddShape(BodyType bodyType, const std::shared_ptr<Actor>& pGameActor, btCollisionShape* pSHape, float mass, const std::string& physicsMaterial);

		//Работа с коллизией объектов
		void RemoveCollisionObject(btCollisionObject* const pBodyToRemove);
		void SendCollisionPairAddEvent(btPersistentManifold const* manifold, const btRigidBody* const body0, const btRigidBody* const body1);
		void SendCollisionPairRemoveEvent(const btRigidBody* const body0, const btRigidBody* const body1);
		static void BulletInternalTickCallback(btDynamicsWorld* const world, const btScalar timeStep);

		//Удаление уничтоженного актера из симуляции
		void DestroyActorDelegate(IEventDataPtr pEventData);

	private:
		//Главная точка управления в bullet
		btDynamicsWorld* m_pDynamicsWorld;
		//Bullet сначала ищет коллизии возможное коллизии между объектами с помощью Broadphase, а потом отправляет всех подозреваемых в CollisionDispatcher на дополнительный обсчет
		btBroadphaseInterface* m_pBroadphase;
		btCollisionDispatcher* m_pDispatcher;
		btConstraintSolver* m_pSolver;
		//Управляет внутренний памятью во время тика менеджера коллизии
		btDefaultCollisionConfiguration* m_pCollisionConfiguration;

		BulletDebugDrawer* m_pDebugDrawer;

		typedef std::map<std::string, float> DensityTable;
		typedef std::map<std::string, MaterialData> MaterialTable;
		DensityTable m_densityTable;
		MaterialTable m_materialTable;

		typedef std::map<ActorId, btRigidBody*> ActorIDToBulletRigidBodyMap;
		ActorIDToBulletRigidBodyMap m_actorIdToRigidBody;

		typedef std::map<btRigidBody const*, ActorId> BulletRigidBodyToActorIDMap;
		BulletRigidBodyToActorIDMap m_rigidBodyToActorId;

		typedef std::pair< btRigidBody const*, btRigidBody const* > CollisionPair;
		typedef std::set< CollisionPair > CollisionPairs;
		CollisionPairs m_previousTickCollisionPairs;

	};

	Physics3D::Physics3D()
		: IGamePhysics3D()
	{
		EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &Physics3D::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);
	}

	Physics3D::~Physics3D()
	{
		EventManager::Get()->RemoveListener(fastdelegate::MakeDelegate(this, &Physics3D::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);

		for (int ii = m_pDynamicsWorld->getNumCollisionObjects() - 1; ii >= 0; --ii)
		{
			btCollisionObject* const obj = m_pDynamicsWorld->getCollisionObjectArray()[ii];

			RemoveCollisionObject(obj);
		}

		m_rigidBodyToActorId.clear();

		if (m_pDynamicsWorld)
		{
			delete m_pDynamicsWorld;
			m_pDynamicsWorld = nullptr;
		}

		if (m_pSolver)
		{
			delete m_pSolver;
			m_pSolver = nullptr;
		}

		if (m_pBroadphase)
		{
			delete m_pBroadphase;
			m_pBroadphase = nullptr;
		}

		if (m_pDispatcher)
		{
			delete m_pDispatcher;
			m_pDispatcher = nullptr;
		}

		if (m_pCollisionConfiguration)
		{
			delete m_pCollisionConfiguration;
			m_pCollisionConfiguration = nullptr;
		}

		if (m_pDebugDrawer)
		{
			delete m_pDebugDrawer;
			m_pDebugDrawer = nullptr;
		}
	}

	bool Physics3D::Initialize()
	{
		auto xmlExtraData = std::static_pointer_cast<BIEngine::XmlExtraData>(BIEngine::ResCache::Get()->GetHandle("config/physics.xml")->GetExtra());
		LoadXml(xmlExtraData->GetRootElement());

		m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
		m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);
		m_pBroadphase = new btDbvtBroadphase();
		m_pSolver = new btSequentialImpulseConstraintSolver();

		m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher,
			m_pBroadphase,
			m_pSolver,
			m_pCollisionConfiguration);

		m_pDebugDrawer = new BulletDebugDrawer();
		m_pDebugDrawer->ReadOptions();

		if (!m_pCollisionConfiguration || !m_pDispatcher || !m_pBroadphase ||
			!m_pSolver || !m_pDynamicsWorld || !m_pDebugDrawer)
		{
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

	void Physics3D::SyncVisibleScene(const std::map<ActorId, std::shared_ptr<Actor>>& actorMap)
	{
		for (ActorIDToBulletRigidBodyMap::const_iterator it = m_actorIdToRigidBody.begin();
			it != m_actorIdToRigidBody.end(); ++it) {
			const ActorId id = it->first;

			const ActorMotionState* const actorMotionState = static_cast<ActorMotionState*>(it->second->getMotionState());
			assert(actorMotionState);

			auto actorIt = actorMap.find(id);
			assert(actorIt != actorMap.end());

			std::shared_ptr<Actor> pGameActor = actorIt->second;
			if (pGameActor && actorMotionState)
			{
				std::shared_ptr<TransformComponent> pTransformComponent = pGameActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
				if (pTransformComponent)
				{
					glm::mat4 trans = glm::mat4(1.0f);
					trans = glm::translate(trans, pTransformComponent->GetPosition());
					glm::vec3 rotationAngles = pTransformComponent->GetRotation();
					const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
						glm::radians(rotationAngles.x),
						glm::vec3(1.0f, 0.0f, 0.0f));
					const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
						glm::radians(rotationAngles.y),
						glm::vec3(0.0f, 1.0f, 0.0f));
					const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
						glm::radians(rotationAngles.z),
						glm::vec3(0.0f, 0.0f, 1.0f));

					const glm::mat4 rotationMatrix = transformZ * transformY * transformX;
					trans *= rotationMatrix;

					if (!glm::all(glm::equal(trans, actorMotionState->m_worldToPositionTransform, glm::epsilon<float>())))
					{
						//We don't pass actorMotionState->m_worldToPositionTransform into Transform Component because it doesn't have a scale
						btTransform btTrans;
						actorMotionState->getWorldTransform(btTrans);
						const glm::vec3 pos = btVector3_to_Vec3(btTrans.getOrigin());
						glm::vec3 rotation;
						btQuaternion qtRot = btTrans.getRotation();

						qtRot.getEulerZYX(rotation.z, rotation.y, rotation.x);
						rotation.x = glm::degrees(rotation.x);
						rotation.y = glm::degrees(rotation.y);
						rotation.z = glm::degrees(rotation.z);
						pTransformComponent->SetPosition(pos);
						pTransformComponent->SetRotation(rotation);

						std::shared_ptr<EvtData_Move_Actor> pEvent = std::make_shared<EvtData_Move_Actor>(id, pTransformComponent->GetPosition(), pTransformComponent->GetRotation());
						EventManager::Get()->QueueEvent(pEvent);
					}
				}
			}
		}
	}

	void Physics3D::OnUpdate(double dt)
	{
		//Bullet будет проводить симуляцию мира, которая должна была пройти за заданный промежуток времени, но будет делать количество проходов не больше чем MAX_PASSES
		//То есть, если dt слишком большое, Bullet может остановиться раньше
		const int MAX_PASSES = 4;
		m_pDynamicsWorld->stepSimulation(dt, MAX_PASSES);
	}

	void Physics3D::DrawRenderDiagnostics()
	{
		m_pDynamicsWorld->debugDrawWorld();
	}

	void Physics3D::AddShape(BodyType bodyType, const std::shared_ptr<Actor>& pGameActor, btCollisionShape* pShape, float mass, const std::string& physicsMaterial)
	{
		assert(pGameActor);

		ActorId actorID = pGameActor->GetId();
		assert(m_actorIdToRigidBody.find(actorID) == m_actorIdToRigidBody.end() && "Actor with more than one physics body?");

		MaterialData material(LookupMaterialData(physicsMaterial));

		btVector3 localInertia(0.f, 0.f, 0.f);
		if (mass > 0.f)
			pShape->calculateLocalInertia(mass, localInertia);


		glm::mat4 transform = glm::mat4(1.0f);
		std::shared_ptr<TransformComponent> pTransformComponent = pGameActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
		assert(pTransformComponent);
		if (pTransformComponent)
		{
			transform = glm::translate(transform, pTransformComponent->GetPosition());
			glm::vec3 rotationAngles = pTransformComponent->GetRotation();
			transform = glm::rotate(transform, glm::radians(rotationAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotationAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
			
		}
		else
		{
			//Нет трансформ компоненты
			return;
		}

		ActorMotionState* const myMotionState = new ActorMotionState(transform);

		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, pShape, localInertia);

		rbInfo.m_restitution = material.m_restitution;
		rbInfo.m_friction = material.m_friction;

		//Bullet считаем объекты недвижемыми, если у них нулевая масса
		if (bodyType != BodyType::DYNAMIC) {
			rbInfo.m_mass = 0;
		}

		btRigidBody* const body = new btRigidBody(rbInfo);

		if (bodyType == BodyType::STATIC) {
			body->setCollisionFlags(body->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE);
		}

		m_pDynamicsWorld->addRigidBody(body);

		m_actorIdToRigidBody[actorID] = body;
		m_rigidBodyToActorId[body] = actorID;
	}

	void Physics3D::AddSphere(float radius, BodyType bodyType, std::weak_ptr<Actor> pGameActor, const std::string& densityStr, const std::string& physicsMaterial)
	{
		std::shared_ptr<Actor> pStrongActor = pGameActor.lock();
		if (!pStrongActor)
		{
			Logger::WriteLog(Logger::LogType::ERROR, "Passed null actor for Physics3D::AddSphere");
			return;
		}

		btSphereShape* const collisionShape = new btSphereShape(radius);

		// calculate absolute mass from specificGravity
		float specificGravity = LookupSpecificGravity(densityStr);
		float const volume = (4.f / 3.f) * M_PI * radius * radius * radius;
		btScalar mass = volume * specificGravity;

		AddShape(bodyType, pStrongActor, collisionShape, mass, physicsMaterial);
	}

	void Physics3D::AddBox(const glm::vec3& dimensions, BodyType bodyType, std::weak_ptr<Actor> pGameActor, const std::string& densityStr, const std::string& physicsMaterial)
	{
		std::shared_ptr<Actor> pStrongActor = pGameActor.lock();
		if (!pStrongActor)
		{
			Logger::WriteLog(Logger::LogType::ERROR, "Passed null actor for Physics3D::AddBox");
			return;
		}

		glm::vec3 dimensionsHalfExtents = dimensions;
		dimensionsHalfExtents.x /= 2;
		dimensionsHalfExtents.y /= 2;
		dimensionsHalfExtents.z /= 2;

		// create the collision body, which specifies the shape of the object
		btBoxShape* const boxShape = new btBoxShape(Vec3_to_btVector3(dimensionsHalfExtents));

		// calculate absolute mass from specificGravity
		float specificGravity = LookupSpecificGravity(densityStr);
		float const volume = dimensions.x * dimensions.y * dimensions.z;
		btScalar mass = volume * specificGravity;

		AddShape(bodyType, pStrongActor, boxShape, mass, physicsMaterial);
	}

	void Physics3D::AddPointCloud(const glm::vec3* verts, int numPoints, BodyType bodyType, std::weak_ptr<Actor> pGameActor, const std::string& densityStr, const std::string& physicsMaterial)
	{
		std::shared_ptr<Actor> pStrongActor = pGameActor.lock();
		if (!pStrongActor)
		{
			Logger::WriteLog(Logger::LogType::ERROR,"Passed null actor for Physics3D::AddPointCloud" );
			return;
		}

		btConvexHullShape* const shape = new btConvexHullShape();

		for (int ii = 0; ii < numPoints; ++ii)
			shape->addPoint(Vec3_to_btVector3(verts[ii]));

		btVector3 aabbMin(0, 0, 0), aabbMax(0, 0, 0);
		shape->getAabb(btTransform::getIdentity(), aabbMin, aabbMax);

		btVector3 const aabbExtents = aabbMax - aabbMin;

		float specificGravity = LookupSpecificGravity(densityStr);
		float const volume = aabbExtents.x() * aabbExtents.y() * aabbExtents.z();
		btScalar const mass = volume * specificGravity;

		AddShape(bodyType, pStrongActor, shape, mass, physicsMaterial);
	}

	void Physics3D::CreateTrigger(std::weak_ptr<Actor> pGameActor, const glm::vec3& dim)
	{
		std::shared_ptr<Actor> pStrongActor = pGameActor.lock();
		if (!pStrongActor)
		{
			Logger::WriteLog(Logger::LogType::ERROR, "Passed null actor for Physics3D::CreateTrigger");
			return;
		}

		btBoxShape* const boxShape = new btBoxShape(Vec3_to_btVector3(dim));

		//Bullet считаем объекты недвижемыми, если у них нулевая масса
		const btScalar mass = 0;

		glm::mat4x4 triggerTrans = glm::mat4x4(1.0f);

		std::shared_ptr<TransformComponent> pTransformComponent = pStrongActor->GetComponent<TransformComponent>(TransformComponent::g_CompId).lock();
		assert(pTransformComponent);
		if (pTransformComponent)
		{
			glm::translate(triggerTrans, pTransformComponent->GetPosition());
		}
		else
			return;

		ActorMotionState* const myMotionState = new ActorMotionState(triggerTrans);

		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, boxShape, btVector3(0, 0, 0));
		btRigidBody* const body = new btRigidBody(rbInfo);

		m_pDynamicsWorld->addRigidBody(body);

		//Триггер ни с чем не контактирует
		body->setCollisionFlags(body->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE);

		m_actorIdToRigidBody[pStrongActor->GetId()] = body;
		m_rigidBodyToActorId[body] = pStrongActor->GetId();
	}

	void Physics3D::RemoveActor(ActorId id)
	{
		if (btRigidBody* const body = FindBulletRigidBody(id))
		{
			RemoveCollisionObject(body);
			m_actorIdToRigidBody.erase(id);
			m_rigidBodyToActorId.erase(body);
		}
	}

	void Physics3D::ApplyForce(const glm::vec3& forceVec, ActorId aid)
	{
		if (btRigidBody* const body = FindBulletRigidBody(aid)) {
			body->setActivationState(DISABLE_DEACTIVATION);
			body->applyCentralImpulse(Vec3_to_btVector3(forceVec));
		}
	}

	void Physics3D::ApplyTorque(const glm::vec3& torque, ActorId aid)
	{
		if (btRigidBody* const body = FindBulletRigidBody(aid))
		{
			body->setActivationState(DISABLE_DEACTIVATION);
			body->applyTorqueImpulse(Vec3_to_btVector3(torque));
		}
	}

	bool Physics3D::KinematicMove(ActorId aid, const glm::vec3& position, const glm::vec3& angles)
	{
		if (btRigidBody* const body = FindBulletRigidBody(aid)) {
			body->setActivationState(DISABLE_DEACTIVATION);

			glm::mat4 trans = glm::mat4(1.0f);
			trans = glm::translate(trans, position);
			trans = glm::rotate(trans, glm::radians(angles.z), glm::vec3(0.0f, 0.0f, 1.0f));
			trans = glm::rotate(trans, glm::radians(angles.y), glm::vec3(0.0f, 1.0f, 0.0f));
			trans = glm::rotate(trans, glm::radians(angles.x), glm::vec3(1.0f, 0.0f, 0.0f));

			body->setWorldTransform(Mat4x4_to_btTransform(trans));
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

		if (closestResults.hasHit())
		{
			info.hasHit = true;

			btVector3 p = btFrom.lerp(btTo, closestResults.m_closestHitFraction);
			info.hitPosition = btVector3_to_Vec3(p);

			if (closestResults.m_collisionObject)
			{
				const btRigidBody* const body = btRigidBody::upcast(closestResults.m_collisionObject);
				info.hitActor = g_pApp->m_pGameLogic->GetActor(FindActorID(body));
			}
		}

		return info;
	}

	void Physics3D::RotateY(ActorId const actorId, float const deltaAngleRadians)
	{
		btRigidBody* pRigidBody = FindBulletRigidBody(actorId);
		assert(pRigidBody);

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
		assert(pRigidBody);

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
			//Складывание рамок
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
		assert(pRigidBody);
		if (!pRigidBody) {
			return glm::vec3(0.0f);
		}

		btVector3 btVel = pRigidBody->getLinearVelocity();
		return btVector3_to_Vec3(btVel);
	}

	void Physics3D::SetVelocity(ActorId actorId, const glm::vec3& vel)
	{
		btRigidBody* pRigidBody = FindBulletRigidBody(actorId);
		assert(pRigidBody);
		if (!pRigidBody) {
			return;
		}

		btVector3 btVel = Vec3_to_btVector3(vel);
		pRigidBody->setLinearVelocity(btVel);
	}

	glm::vec3 Physics3D::GetAngularVelocity(ActorId actorId) const
	{
		btRigidBody* pRigidBody = FindBulletRigidBody(actorId);
		assert(pRigidBody);

		if (!pRigidBody)
			return glm::vec3(0.0f);

		btVector3 btVel = pRigidBody->getAngularVelocity();
		return btVector3_to_Vec3(btVel);
	}

	void Physics3D::SetAngularVelocity(ActorId actorId, const glm::vec3& vel)
	{
		btRigidBody* pRigidBody = FindBulletRigidBody(actorId);
		assert(pRigidBody);

		if (!pRigidBody)
			return;

		btVector3 btVel = Vec3_to_btVector3(vel);
		pRigidBody->setAngularVelocity(btVel);
	}

	void Physics3D::SetPosition(const ActorId id, const glm::vec3& position)
	{
		btRigidBody* pRigidBody = FindBulletRigidBody(id);
		assert(pRigidBody);

		if (pRigidBody) {
			btVector3 btVec = Vec3_to_btVector3(position);
			pRigidBody->translate(btVec);
		}
	}

	glm::vec3 Physics3D::GetPosition(const ActorId id) const
	{
		btRigidBody* pRigidBody = FindBulletRigidBody(id);
		assert(pRigidBody);

		if (pRigidBody) {
			btVector3 pos = pRigidBody->getWorldTransform().getOrigin();
			return btVector3_to_Vec3(pos);
		}

		return glm::vec3(0.0f);
	}

	void Physics3D::LoadXml(tinyxml2::XMLElement* pRoot)
	{
		assert(pRoot);

		tinyxml2::XMLElement* pParentNode = pRoot->FirstChildElement("PhysicsMaterials");
		assert(pParentNode);
		for (tinyxml2::XMLElement* pNode = pParentNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
		{
			double restitution = 0;
			double friction = 0;
			pNode->QueryDoubleAttribute("restitution", &restitution);
			pNode->QueryDoubleAttribute("friction", &friction);
			m_materialTable.insert(std::make_pair(pNode->Value(), MaterialData((float)restitution, (float)friction)));
		}

		pParentNode = pRoot->FirstChildElement("DensityTable");
		assert(pParentNode);
		for (tinyxml2::XMLElement* pNode = pParentNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
		{
			m_densityTable.insert(std::make_pair(pNode->Value(), (float)atof(pNode->FirstChild()->Value())));
		}
	}

	float Physics3D::LookupSpecificGravity(const std::string& densityStr)
	{
		float density = 0;
		auto densityIt = m_densityTable.find(densityStr);
		if (densityIt != m_densityTable.end())
			density = densityIt->second;

		return density;
	}

	MaterialData Physics3D::LookupMaterialData(const std::string& materialStr)
	{
		auto materialIt = m_materialTable.find(materialStr);
		if (materialIt != m_materialTable.end())
			return materialIt->second;
		else
			return MaterialData(0.0, 0.0);
	}

	btRigidBody* Physics3D::FindBulletRigidBody(ActorId const id) const
	{
		ActorIDToBulletRigidBodyMap::const_iterator found = m_actorIdToRigidBody.find(id);
		if (found != m_actorIdToRigidBody.end())
			return found->second;

		return nullptr;
	}

	ActorId Physics3D::FindActorID(const btRigidBody* const pBody) const
	{
		BulletRigidBodyToActorIDMap::const_iterator found = m_rigidBodyToActorId.find(pBody);
		if (found != m_rigidBodyToActorId.end())
			return found->second;

		return Actor::INVALID_ACTOR_ID;
	}

	void Physics3D::RemoveCollisionObject(btCollisionObject* const pBodyToRemove)
	{
		//Сначала удаляем объект из симуляции
		m_pDynamicsWorld->removeCollisionObject(pBodyToRemove);

		//Потом удаляем объект из списка столкновений
		for (CollisionPairs::iterator it = m_previousTickCollisionPairs.begin();
			it != m_previousTickCollisionPairs.end(); )
		{
			CollisionPairs::iterator next = it;
			++next;

			if (it->first == pBodyToRemove || it->second == pBodyToRemove)
			{
				SendCollisionPairRemoveEvent(it->first, it->second);
				m_previousTickCollisionPairs.erase(it);
			}

			it = next;
		}

		if (btRigidBody* const body = btRigidBody::upcast(pBodyToRemove))
		{
			// delete the components of the object
			delete body->getMotionState();
			delete body->getCollisionShape();
			delete body->getUserPointer();
			delete body->getUserPointer();

			for (int ii = body->getNumConstraintRefs() - 1; ii >= 0; --ii)
			{
				btTypedConstraint* const constraint = body->getConstraintRef(ii);
				m_pDynamicsWorld->removeConstraint(constraint);
				delete constraint;
			}
		}

		delete pBodyToRemove;
	}

	void Physics3D::SendCollisionPairAddEvent(btPersistentManifold const* manifold, const btRigidBody* const body0, const btRigidBody* const body1)
	{
		if (body0->getUserPointer() || body1->getUserPointer())
		{
			//Только триггер имеет ненулевой userPointer. Найдем кто из пришедших тех им является
			const btRigidBody * triggerBody, * otherBody;

			if (body0->getUserPointer()) {
				triggerBody = body0;
				otherBody = body1;
			}
			else {
				otherBody = body0;
				triggerBody = body1;
			}

			int const triggerId = *static_cast<int*>(triggerBody->getUserPointer());
			std::shared_ptr<EvtData_Phys3DTrigger_Enter> pEvent = std::make_shared<EvtData_Phys3DTrigger_Enter>(triggerId, FindActorID(otherBody));
			EventManager::Get()->QueueEvent(pEvent);
		}
		else
		{
			ActorId const id0 = FindActorID(body0);
			ActorId const id1 = FindActorID(body1);

			if (id0 == Actor::INVALID_ACTOR_ID || id1 == Actor::INVALID_ACTOR_ID) {
				return;
			}

			// this pair of colliding objects is new.  send a collision-begun event
			std::vector<glm::vec3> collisionPoints;
			glm::vec3 sumNormalForce;
			glm::vec3 sumFrictionForce;

			for (int pointIdx = 0; pointIdx < manifold->getNumContacts(); ++pointIdx)
			{
				btManifoldPoint const& point = manifold->getContactPoint(pointIdx);

				collisionPoints.push_back(btVector3_to_Vec3(point.getPositionWorldOnB()));

				sumNormalForce += btVector3_to_Vec3(point.m_combinedRestitution * point.m_normalWorldOnB);
				sumFrictionForce += btVector3_to_Vec3(point.m_combinedFriction * point.m_lateralFrictionDir1);
			}

			std::shared_ptr<EvtData_Phys3DCollision> pEvent = std::make_shared<EvtData_Phys3DCollision>(id0, id1, sumNormalForce, sumFrictionForce.length(), collisionPoints);
			EventManager::Get()->QueueEvent(pEvent);
		}
	}

	void Physics3D::SendCollisionPairRemoveEvent(const btRigidBody* const body0, const btRigidBody* const body1)
	{
		if (body0->getUserPointer() || body1->getUserPointer()) {
			//Только триггер имеет ненулевой userPointer. Найдем кто из пришедших тех им является
			const btRigidBody* triggerBody, * otherBody;

			if (body0->getUserPointer()) {
				triggerBody = body0;
				otherBody = body1;
			}
			else {
				otherBody = body0;
				triggerBody = body1;
			}

			int const triggerId = *static_cast<int*>(triggerBody->getUserPointer());
			std::shared_ptr<EvtData_Phys3DTrigger_Leave> pEvent = std::make_shared<EvtData_Phys3DTrigger_Leave>(triggerId, FindActorID(otherBody));
			EventManager::Get()->QueueEvent(pEvent);
		}
		else
		{
			ActorId const id0 = FindActorID(body0);
			ActorId const id1 = FindActorID(body1);

			if (id0 == Actor::INVALID_ACTOR_ID || id1 == Actor::INVALID_ACTOR_ID) {
				return;
			}

			std::shared_ptr<EvtData_Phys3DSeparation> pEvent = std::make_shared<EvtData_Phys3DSeparation>(id0, id1);
			EventManager::Get()->QueueEvent(pEvent);
		}
	}

	void Physics3D::BulletInternalTickCallback(btDynamicsWorld* const world, const btScalar timeStep)
	{
		assert(world);

		assert(world->getWorldUserInfo());
		Physics3D* const bulletPhysics = static_cast<Physics3D*>(world->getWorldUserInfo());

		CollisionPairs currentTickCollisionPairs;

		//Ищем столкновения
		btDispatcher* const dispatcher = world->getDispatcher();
		for (int manifoldIdx = 0; manifoldIdx < dispatcher->getNumManifolds(); ++manifoldIdx)
		{
			
			//Получаем данные, относящиеся к столкновению между двумя столкновения (вот, что значит manifold)
			const btPersistentManifold* const manifold = dispatcher->getManifoldByIndexInternal(manifoldIdx);
			assert(manifold);

			const btRigidBody* const body0 = static_cast<const btRigidBody*>(manifold->getBody0());
			const btRigidBody* const body1 = static_cast<const btRigidBody*>(manifold->getBody1());

			//Всегда создаем пары в предсказуемом порядке
			bool const swapped = body0 > body1;

			const btRigidBody* const sortedBodyA = swapped ? body1 : body0;
			const btRigidBody* const sortedBodyB = swapped ? body0 : body1;

			CollisionPair const thisPair = std::make_pair(sortedBodyA, sortedBodyB);
			currentTickCollisionPairs.insert(thisPair);

			if (bulletPhysics->m_previousTickCollisionPairs.find(thisPair) == bulletPhysics->m_previousTickCollisionPairs.end())
			{
				//Это новый контакт - отправляем событие
				bulletPhysics->SendCollisionPairAddEvent(manifold, body0, body1);
			}
		}

		CollisionPairs removedCollisionPairs;

		std::set_difference(bulletPhysics->m_previousTickCollisionPairs.begin(), bulletPhysics->m_previousTickCollisionPairs.end(),
			currentTickCollisionPairs.begin(), currentTickCollisionPairs.end(),
			std::inserter(removedCollisionPairs, removedCollisionPairs.begin()));

		for (CollisionPairs::const_iterator it = removedCollisionPairs.begin(),
			end = removedCollisionPairs.end(); it != end; ++it)
		{
			btRigidBody const* const body0 = it->first;
			btRigidBody const* const body1 = it->second;

			bulletPhysics->SendCollisionPairRemoveEvent(body0, body1);
		}

		// the current tick becomes the previous tick.  this is the way of all things.
		bulletPhysics->m_previousTickCollisionPairs = currentTickCollisionPairs;
	}

	void Physics3D::DestroyActorDelegate(IEventDataPtr pEventData)
	{
		std::shared_ptr<EvtData_Destroy_Actor> pCastEventData = std::static_pointer_cast<EvtData_Destroy_Actor>(pEventData);
		RemoveActor(pCastEventData->GetId());
	}


	IGamePhysics3D* CreateGamePhysics3D()
	{
		IGamePhysics3D* pGamePhysics3D = new Physics3D();
		assert(pGamePhysics3D);
		return pGamePhysics3D;
	}

}