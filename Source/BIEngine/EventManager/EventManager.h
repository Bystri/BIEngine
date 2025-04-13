#pragma once

#include <strstream>
#include <list>
#include <map>
#include <memory>

#include "../Network/Serialization.h"
#include "../Utilities/GenericObjectFactory.h"
#include "../3rdParty/FastDelegate/FastDelegate.h"

namespace BIEngine {

using EventType = unsigned long;

class IEventData {
public:
   virtual const EventType& GetEventType() const = 0;
   virtual float GetTimeStamp() const = 0;

   // Методы передачи данных в потоке

   // Отправляет в поток вывода сериализованное представление события
   virtual void Write(OutputMemoryBitStream& os) const = 0;
   // Отправляет в поток ввода десериализованное представление события
   virtual void Read(InputMemoryBitStream& is) = 0;

   virtual const char* GetName() const = 0;
};

using IEventDataPtr = std::shared_ptr<IEventData>;
using EventListenerDelegate = fastdelegate::FastDelegate1<IEventDataPtr>;

class BaseEventData : public IEventData {
public:
   static const EventType sk_EventType;

   explicit BaseEventData(const float timeStamp = 0.0f)
      : m_timeStamp(timeStamp)
   {
   }

   virtual ~BaseEventData() {}

   virtual const EventType& GetEventType() const override { return sk_EventType; }

   float GetTimeStamp() const override { return m_timeStamp; }

   virtual void Write(OutputMemoryBitStream& out) const override {}

   virtual void Read(InputMemoryBitStream& in) override {}

   virtual const char* GetName() const { return "Unknown"; }

private:
   const float m_timeStamp;
};

extern GenericObjectFactory<IEventData, EventType> g_eventFactory;

// Менеджер событий является Singleton-объектом, так как должен быть доступен практически из каждой системы
class EventManager {
   using EventListenerList = std::list<EventListenerDelegate>;
   using EventListenerMap = std::map<EventType, EventListenerList>;
   using EventQueue = std::list<IEventDataPtr>;

   static const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

public:
   static const long long INFINITE_TIME = 0xffffffff;

   static EventManager* Get();

   virtual bool AddListener(const EventListenerDelegate& eventDelegate, const EventType& type);
   virtual bool RemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type);

   // Выполнение события сразу, не дожидаясь выполнения событий из очереди
   virtual bool TriggerEvent(const IEventDataPtr& pEvent) const;
   // Поставить событие в очередь на выполнение
   virtual bool QueueEvent(const IEventDataPtr& pEvent);
   virtual bool AbortEvent(const EventType& type, bool allOfType = false);
   // Выполнить соытия из очереди. Можно поставить ограничение времени выполнения на текущий тик оновления.
   virtual bool TickUpdate(long long maxMillis = INFINITE_TIME);

private:
   explicit EventManager();
   virtual ~EventManager();

private:
   EventListenerMap m_eventListeners;
   EventQueue m_queue[EVENTMANAGER_NUM_QUEUES];
   int m_activeQueue;
};

} // namespace BIEngine
