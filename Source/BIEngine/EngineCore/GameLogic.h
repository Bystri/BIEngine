#pragma once

#include "../StdLib/SharedPtr.h"
#include "../Actors/ActorFactory.h"
#include "../UserInterface/HumanView.h"
#include "../Physics/Physics2D.h"
#include "../Physics/Physics3D.h"
#include "../Navigation/NavWorld.h"

namespace BIEngine {

class NetworkManager;

class GameLogic {
   friend class GameApp;

public:
   using ActorMap = HashMap<ActorId, SharedPtr<Actor>>;

   GameLogic();
   virtual ~GameLogic();

   GameLogic(const GameLogic& orig) = delete;
   GameLogic& operator=(const GameLogic& rhs) = delete;

   SharedPtr<IGamePhysics2D> GetGamePhysics2D() const { return m_pPhysics2D; }

   SharedPtr<IGamePhysics3D> GetGamePhysics3D() const { return m_pPhysics3D; }

   UniquePtr<NavWorld>& GetNavWorld() { return m_pNavWorld; }

   UniquePtr<NetworkManager>& GetNetworkManager();

   virtual bool Init();

   virtual void Terminate();

   bool LoadLevel(const String& path);

   bool IsLevelLoaded() const { return m_bIsLevelLoaded; };

   SharedPtr<Actor> CreateActor(tinyxml2::XMLElement* pRoot, const glm::vec3* const pPosition = nullptr, const glm::vec3* const pRotation = nullptr);
   // Принимает на вход XML-структуру актера, компоненты в котором будут заменены или добавлены.
   void ModifyActor(ActorId actorId, tinyxml2::XMLElement* pOverrides);
   // Является ответчиком на запрос об уничтожении актера
   void RequestDestroyActorDelegate(IEventDataPtr pEventData);
   virtual void DestroyActor(const ActorId actorId);

   virtual void AddGameView(SharedPtr<IGameView> pView);
   virtual void RemoveGameView(SharedPtr<IGameView> pView);

   virtual void OnUpdate(GameTimer& gt);
   virtual void OnRender(const GameTimer& gt);
   virtual void OnRenderDebug(const GameTimer& gt);

   SharedPtr<Actor> GetActor(ActorId id) const;

   int GetNumActors() const { return m_actors.Size(); };

   const ActorMap& GetActors() const { return m_actors; }

   // Обработчики событий устройств ввода
   void OnPointerMove(float xpos, float ypos);
   void SetPointerButton(int button, bool state);
   void SetKey(int key, int scancode, bool state);

protected:
   // Должен быть переопределен дочерним классом, если нужно сделать что-то особое во время загрузки мира.
   // Такая необохдимость в отдельной функции возникает из-за выполнение скриптов перед и после загрузки мира.
   virtual bool LoadLevelDelegate(tinyxml2::XMLElement* pRoot) { return true; }

protected:
   EventManager::DelegateHandler m_requestDestroyActorDelegateHandler;

   bool m_bIsLevelLoaded;

   GameViewList m_gameViews;

   ActorMap m_actors;
   ActorFactory* m_pActorFactory;

   SharedPtr<IGamePhysics2D> m_pPhysics2D;
   SharedPtr<IGamePhysics3D> m_pPhysics3D;
   UniquePtr<NetworkManager> m_pNetworkManager;
   UniquePtr<NavWorld> m_pNavWorld;
};

} // namespace BIEngine
