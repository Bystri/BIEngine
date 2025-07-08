#pragma once

#include <strstream>
#include <unordered_map>
#include <memory>
#include <functional>

#include "../StdLib/List.h"
#include "../StdLib/DynamicArray.h"
#include "../Network/Serialization.h"
#include "../Utilities/GenericObjectFactory.h"

namespace BIEngine {

using EventType = uint32_t;

class IEventData {
public:
   virtual const EventType& GetEventType() const = 0;

   virtual void Write(OutputMemoryBitStream& os) const = 0;
   virtual void Read(InputMemoryBitStream& is) = 0;

   virtual const char* GetName() const = 0;
};

using IEventDataPtr = std::shared_ptr<IEventData>;
using EventListenerDelegate = std::function<void(IEventDataPtr)>;

#define MAKE_EVENT_DELEGATE_FROM_MEMBER_FUNC(func) \
   std::bind(&func, this, std::placeholders::_1)

class BaseEventData : public IEventData {
public:
   static const EventType sk_EventType;

   BaseEventData()
   {
   }

   virtual ~BaseEventData() {}

   virtual const EventType& GetEventType() const override { return sk_EventType; }

   virtual void Write(OutputMemoryBitStream& out) const override {}

   virtual void Read(InputMemoryBitStream& in) override {}

   virtual const char* GetName() const { return "Unknown"; }
};

extern GenericObjectFactory<IEventData, EventType> g_eventFactory;

// Менеджер событий является Singleton-объектом, так как должен быть доступен практически из каждой системы
class EventManager {
public:
   using DelegateHandler = uint64_t;

private:
   using EventListenerStorage = DynamicArray<EventListenerDelegate>;
   using EventHandlerStorage = DynamicArray<DelegateHandler>;
   using EventQueue = List<IEventDataPtr>;

   static const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

public:
   static const long long INFINITE_TIME = 0xffffffff;

   static EventManager* Get();

   virtual DelegateHandler AddListener(EventListenerDelegate&& eventDelegate, const EventType& type);
   virtual bool RemoveListener(DelegateHandler handler);

   // Выполнение события сразу, не дожидаясь выполнения событий из очереди
   virtual bool TriggerEvent(const IEventDataPtr& pEvent) const;
   // Поставить событие в очередь на выполнение
   virtual bool QueueEvent(const IEventDataPtr& pEvent);
   virtual bool AbortEvent(const EventType& type, bool allOfType = false);
   // Выполнить соытия из очереди. Можно поставить ограничение времени выполнения на текущий тик оновления.
   virtual bool TickUpdate(long long maxMillis = INFINITE_TIME);

private:
   EventManager();
   virtual ~EventManager();

private:
   uint32_t m_nextId = 0;
   std::unordered_map<EventType, EventListenerStorage> m_eventListeners;
   std::unordered_map<EventType, EventHandlerStorage> m_delegateHandlers;
   EventQueue m_queue[EVENTMANAGER_NUM_QUEUES];
   int m_activeQueue;
};

} // namespace BIEngine
