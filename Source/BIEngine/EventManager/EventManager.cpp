#include "EventManager.h"

#include <chrono>
#include <iostream>

#include "../Utilities/Logger.h"
#include "../EngineCore/Assert.h"

namespace BIEngine {

const EventType BaseEventData::sk_EventType(0x00000000);

static EventManager* g_pEventMgr = nullptr;
GenericObjectFactory<IEventData, EventType> g_eventFactory;

EventManager* EventManager::Get()
{
   if (!g_pEventMgr) {
      g_pEventMgr = new EventManager();
   }

   return g_pEventMgr;
}

EventManager::EventManager()
   : m_eventListeners(), m_queue(), m_activeQueue(0)
{
   if (g_pEventMgr) {
      Logger::WriteLog(Logger::LogType::ERROR, "Attempting to create two global event managers! The old one will be destroyed and overwritten with this one.\n");
      delete g_pEventMgr;
   }

   g_pEventMgr = this;
}

EventManager::~EventManager()
{
   if (g_pEventMgr == this) {
      delete g_pEventMgr;
      g_pEventMgr = nullptr;
   }
}

EventManager::DelegateHandler EventManager::AddListener(EventListenerDelegate&& eventDelegate, const EventType& type)
{
   auto& eventListenerList = m_eventListeners[type];
   auto& delegateHandlersList = m_delegateHandlers[type];

   eventListenerList.PushBack(std::move(eventDelegate));

   DelegateHandler newHandler = type;
   newHandler <<= 32;
   newHandler |= m_nextId;
   delegateHandlersList.PushBack(newHandler);

   ++m_nextId;

   return newHandler;
}

bool EventManager::RemoveListener(const EventManager::DelegateHandler handler)
{
   const EventType type = handler >> 32;

   auto findIt = m_delegateHandlers.find(type);

   if (findIt == m_delegateHandlers.end()) {
      return false;
   }

   auto& handlersStorage = findIt->second;

   auto itr = handlersStorage.Begin();
   for (int idx = 0; itr != handlersStorage.End(); ++itr, ++idx) {
      if ((*itr) == handler) {
         auto& listenersStorage = m_eventListeners[type];
         listenersStorage.Erase(listenersStorage.Begin() + idx);
         handlersStorage.Erase(itr);

         return true;
      }
   }

   return false;
}

bool EventManager::TriggerEvent(const IEventDataPtr& pEvent) const
{
   bool processed = false;

   auto findIt = m_eventListeners.find(pEvent->GetEventType());
   if (findIt != m_eventListeners.end()) {
      const EventListenerStorage& eventListenerList = findIt->second;
      for (auto itr = eventListenerList.CBegin(); itr != eventListenerList.CEnd(); ++itr) {
         EventListenerDelegate listener = (*itr);
         listener(pEvent);
         processed = true;
      }
   }

   return processed;
}

bool EventManager::QueueEvent(const IEventDataPtr& pEvent)
{
   Assert(m_activeQueue >= 0 && m_activeQueue < EVENTMANAGER_NUM_QUEUES, "Incorrect index of current event queue. Got %d but [%d, %d] allowed", m_activeQueue, 0, EVENTMANAGER_NUM_QUEUES);
   if (m_activeQueue < 0 || m_activeQueue >= EVENTMANAGER_NUM_QUEUES) {
      return false;
   }

   auto findIt = m_eventListeners.find(pEvent->GetEventType());
   if (findIt != m_eventListeners.end()) {
      m_queue[m_activeQueue].PushBack(pEvent);
      return true;
   }

   return false;
}

bool EventManager::AbortEvent(const EventType& inType, bool allOfType)
{
   Assert(m_activeQueue >= 0 && m_activeQueue < EVENTMANAGER_NUM_QUEUES, "Incorrect index of current event queue. Got %d but [%d, %d] allowed", m_activeQueue, 0, EVENTMANAGER_NUM_QUEUES);
   if (m_activeQueue < 0 || m_activeQueue >= EVENTMANAGER_NUM_QUEUES) {
      return false;
   }

   bool success = false;
   auto findItr = m_eventListeners.find(inType);

   if (findItr != m_eventListeners.end()) {
      EventQueue& eventQueue = m_queue[m_activeQueue];
      auto itr = eventQueue.Begin();
      while (itr != eventQueue.End())
      {
         if ((*itr)->GetEventType() != inType) {
            ++itr;
            continue;
         }

         itr = eventQueue.Erase(itr);
         if (!allOfType) {
            break;
         }
         success = true;
      }
   }

   return success;
}

bool EventManager::TickUpdate(long long maxMillis)
{
   long long currMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
   long long maxMs = ((maxMillis == EventManager::INFINITE_TIME) ? EventManager::INFINITE_TIME : currMs + maxMillis);

   int queueToProcess = m_activeQueue;
   m_activeQueue = (m_activeQueue + 1) % EVENTMANAGER_NUM_QUEUES;
   m_queue[m_activeQueue].Clear();

   while (!m_queue[queueToProcess].Empty()) {
      IEventDataPtr pEvent = m_queue[queueToProcess].Front();
      m_queue[queueToProcess].PopFront();

      const EventType& eventType = pEvent->GetEventType();

      auto findIt = m_eventListeners.find(eventType);
      if (findIt != m_eventListeners.end()) {
         const EventListenerStorage& eventListeners = findIt->second;

         for (auto& listener : eventListeners) {
            listener(pEvent);
         }
      }

      currMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
      if (maxMillis != EventManager::INFINITE_TIME && currMs >= maxMs) {
         Logger::WriteLog(Logger::LogType::MESSAGE, "Aborting event processing: time ran out");
         break;
      }
   }

   bool queueFlushed = m_queue[queueToProcess].Empty();
   if (!queueFlushed) {
      while (!m_queue[queueToProcess].Empty()) {
         IEventDataPtr pEvent = m_queue[queueToProcess].Back();
         m_queue[queueToProcess].PopBack();
         m_queue[m_activeQueue].PushFront(pEvent);
      }
   }

   return queueFlushed;
}

} // namespace BIEngine
