﻿#include "GameLogic.h"

#include "../Utilities/Logger.h"
#include "../ProcessManager/ProcessManager.h"

namespace BIEngine {

GameLogic::GameLogic()
   : m_gameViews(), m_bIsLevelLoaded(false),
     m_actors(), m_pActorFactory(new ActorFactory)
{
}

GameLogic::~GameLogic()
{
   if (m_pActorFactory) {
      delete m_pActorFactory;
      m_pActorFactory = nullptr;
   }
}

bool GameLogic::Init()
{
   ProcessManager::Create();

   EventManager::Get()->AddListener(fastdelegate::MakeDelegate(this, &GameLogic::RequestDestroyActorDelegate), EvtData_Request_Destroy_Actor::sk_EventType);

   return true;
}

bool GameLogic::LoadLevel(const std::string& path)
{
   const std::string xmlWorldPath = path + "/World.xml";

   auto resourceHandle = ResCache::Get()->GetHandle(xmlWorldPath);

   if (!resourceHandle)
      return false;

   auto levelXmlData = std::dynamic_pointer_cast<XmlExtraData>(resourceHandle->GetExtra());
   Assert(levelXmlData != nullptr, "level xml data did not loaded");

   if (levelXmlData == nullptr) {
      return false;
   }

   tinyxml2::XMLElement* pRoot = levelXmlData->GetRootElement();
   if (!pRoot) {
      Assert(false, "Failed to load level-xml file %s", xmlWorldPath.c_str());
      return false;
   }

   // Необязательные скрипты для выполнения перед и после загрузки уровня
   const char* preLoadScript = nullptr;
   const char* postLoadScript = nullptr;

   tinyxml2::XMLElement* pScriptElement = pRoot->FirstChildElement("Script");
   if (pScriptElement) {
      preLoadScript = pScriptElement->Attribute("preLoad");
      postLoadScript = pScriptElement->Attribute("postLoad");
   }

   // Выполнения скрипта перед загрузкой уровня
   if (preLoadScript) {
      // Загрузки ресурсов уже автоматически исполняет скрипт, поэтому нам ничего делать не нужно
      auto pResourceHandle = ResCache::Get()->GetHandle(preLoadScript);
   }

   // Загрузка актеров
   tinyxml2::XMLElement* pActorsNode = pRoot->FirstChildElement("Actors");
   if (pActorsNode) {
      for (tinyxml2::XMLElement* pNode = pActorsNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
         auto pActor = CreateActor(pNode);
      }
   }

   if (!LoadLevelDelegate(pRoot))
      return false;

   // Выполнения скрипта после загрузки уровня
   if (postLoadScript) {
      // Загрузки ресурсов уже автоматически исполняет скрипт, поэтому нам ничего делать не нужно
      auto pResourceHandle = ResCache::Get()->GetHandle(postLoadScript);
   }

   m_bIsLevelLoaded = true;

   for (auto& actor : m_actors) {
      actor.second->OnLevelLoaded();
   }

   for (auto& actor : m_actors) {
      actor.second->Activate();
   }

   // TODO: Кидать событие загрузки уровня
   return true;
}

void GameLogic::AddGameView(std::shared_ptr<IGameView> pView)
{
   m_gameViews.push_back(pView);
}

void GameLogic::RemoveGameView(std::shared_ptr<IGameView> pView)
{
   for (auto itr = m_gameViews.begin(); itr != m_gameViews.end(); ++itr) {
      if (*itr == pView) {
         m_gameViews.erase(itr);
         return;
      }
   }
}

void GameLogic::OnUpdate(GameTimer& gt)
{
   EventManager::Get()->TickUpdate();

   m_pPhysics2D->OnUpdate(gt);
   m_pPhysics2D->SyncVisibleScene(m_actors);

   m_pPhysics3D->BeforeUpdate(m_actors);
   m_pPhysics3D->OnUpdate(gt);
   m_pPhysics3D->AfterUpdate(m_actors);

   ProcessManager::Get()->UpdateProcesses(gt);

   for (const auto& actor : m_actors) {
      actor.second->OnUpdate(gt);
   }

   for (const auto view : m_gameViews) {
      view->OnUpdate(gt);
   }
}

void GameLogic::OnRender(const GameTimer& gt)
{
   for (const auto view : m_gameViews) {
      view->OnPreRender(gt);
   }

   for (const auto& actor : m_actors) {
      actor.second->OnRenderObject(gt);
   }

   m_pPhysics3D->DrawRenderDiagnostics();

   for (const auto view : m_gameViews) {
      view->OnPostRender(gt);
   }
}

std::shared_ptr<Actor> GameLogic::GetActor(ActorId id) const
{
   auto it = m_actors.find(id);

   if (it == m_actors.end()) {
      return std::shared_ptr<Actor>();
   }

   return it->second;
}

void GameLogic::OnPointerMove(float xpos, float ypos)
{
   for (const auto view : m_gameViews) {
      view->OnPointerMove(xpos, ypos);
   }
}

void GameLogic::SetPointerButton(int button, bool state)
{
   for (const auto view : m_gameViews) {
      view->SetPointerButton(button, state);
   }
}

void GameLogic::SetKey(int key, int scancode, bool state)
{
   for (const auto view : m_gameViews) {
      view->SetKey(key, scancode, state);
   }
}

static void gameLogicInsertActorToMap(GameLogic::ActorMap& actors, std::shared_ptr<Actor> pActor)
{
   actors.insert(std::make_pair(pActor->GetId(), pActor));
   for (const auto& child : pActor->GetChildren()) {
      gameLogicInsertActorToMap(actors, child);
   }
}

std::shared_ptr<Actor> GameLogic::CreateActor(tinyxml2::XMLElement* pRoot, const glm::vec3* const pPosition, const glm::vec3* const pRotation)
{
   std::shared_ptr<Actor> pActor = m_pActorFactory->CreateActor(pRoot, pPosition, pRotation);
   if (pActor) {
      gameLogicInsertActorToMap(m_actors, pActor);

      std::shared_ptr<EvtData_Actor_Created> pEvent = std::make_shared<EvtData_Actor_Created>(pActor->GetId());
      EventManager::Get()->TriggerEvent(pEvent);

      return pActor;
   } else {
      Logger::WriteLog(Logger::LogType::ERROR, "Couldn't create actor");
      return std::shared_ptr<Actor>();
   }
}

void GameLogic::ModifyActor(ActorId actorId, tinyxml2::XMLElement* pOverrides)
{
   auto itr = m_actors.find(actorId);
   if (itr != m_actors.end())
      m_pActorFactory->ModifyActor(itr->second, pOverrides);
   else
      Logger::WriteLog(Logger::LogType::ERROR, "Attempt to change a non-existent actor");
}

void GameLogic::RequestDestroyActorDelegate(IEventDataPtr pEventData)
{
   std::shared_ptr<EvtData_Request_Destroy_Actor> pCastEventData = std::static_pointer_cast<EvtData_Request_Destroy_Actor>(pEventData);
   DestroyActor(pCastEventData->GetActorId());
}

static void gameLogicDestroyActorFromMap(GameLogic::ActorMap& actors, const ActorId actorId)
{
   auto findIt = actors.find(actorId);

   if (findIt == actors.end()) {
      return;
   }

   for (auto& child : findIt->second->GetChildren()) {
      gameLogicDestroyActorFromMap(actors, child->GetId());
   }

   findIt->second->Destroy();
   actors.erase(findIt);
}

void GameLogic::DestroyActor(const ActorId actorId)
{
   // TODO: нам необходимо создать и триггерить событие фактического уничтожения актера, чтобы все системы успели выполнить подготовку к уничтожению
   std::shared_ptr<EvtData_Destroy_Actor> pEvent = std::make_shared<EvtData_Destroy_Actor>(actorId);
   EventManager::Get()->TriggerEvent(pEvent);

   gameLogicDestroyActorFromMap(m_actors, actorId);
}
} // namespace BIEngine
