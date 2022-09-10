#pragma once

#include "../3rdParty/FastDelegate/FastDelegate.h"

#include <strstream>
#include <list>
#include <map>
#include <memory>

namespace BIEngine
{

	typedef unsigned long EventType;

	class IEventData
	{
	public:
		virtual const EventType& GetEventType() const = 0;
		virtual float GetTimeStamp() const = 0;

		//������ �������� ������ � ������

		//���������� � ����� ������ ��������������� ������������� �������
		virtual void Serialize(std::ostrstream& os) const = 0;
		//���������� � ����� ����� ����������������� ������������� �������
		virtual void Deserialize(std::istrstream& is) const = 0;

		virtual const char* GetName() const = 0;
	};

	typedef std::shared_ptr<IEventData> IEventDataPtr;
	typedef fastdelegate::FastDelegate1<IEventDataPtr> EventListenerDelegate;


	class EventFactory
	{
		typedef IEventData* (*ObjectCreationFunction)();
		std::map<EventType, ObjectCreationFunction> m_creationFunctions;

	public:
		template <class SubClass>
		bool Register(EventType id)
		{
			auto findIt = m_creationFunctions.find(id);
			if (findIt == m_creationFunctions.end())
			{
				m_creationFunctions[id] = &GenericObjectCreationFunction<IEventData, SubClass>;
				return true;
			}

			return false;
		}

		IEventData* Create(EventType id)
		{
			auto findIt = m_creationFunctions.find(id);
			if (findIt != m_creationFunctions.end())
			{
				ObjectCreationFunction pFunc = findIt->second;
				return pFunc();
			}

			return NULL;
		}
	};

	extern EventFactory g_eventFactory;
#define REGISTER_EVENT(eventClass) g_eventFactory.Register<eventClass>(eventClass::sk_EventType)
#define CREATE_EVENT(eventType) g_eventFactory.Create(eventType)

	class BaseEventData : public IEventData
	{
	public:
		explicit BaseEventData(const float timeStamp = 0.0f)
			: m_timeStamp(timeStamp)
		{}

		virtual ~BaseEventData() {}

		virtual const EventType& GetEventType() const = 0;

		float GetTimeStamp() const { return m_timeStamp; }
		virtual void Serialize(std::ostrstream& out) const {}
		virtual void Deserialize(std::istrstream& in) const {}

	private:
		const float m_timeStamp;
	};


	//�������� ������� �������� Singleton-��������, ��� ��� ������ ���� �������� ����������� �� ������ �������
	class EventManager
	{
		typedef std::list<EventListenerDelegate> EventListenerList;
		typedef std::map<EventType, EventListenerList> EventListenerMap;
		typedef std::list<IEventDataPtr> EventQueue;

		static const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

	public:
		static const long long INFINITE_TIME = 0xffffffff;

		static EventManager* EventManager::Get();

		virtual bool AddListener(const EventListenerDelegate& eventDelegate, const EventType& type);
		virtual bool RemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type);

		//���������� ������� �����, �� ��������� ���������� ������� �� �������
		virtual bool TriggerEvent(const IEventDataPtr& pEvent) const;
		//��������� ������� � ������� �� ����������
		virtual bool QueueEvent(const IEventDataPtr& pEvent);
		virtual bool AbortEvent(const EventType& type, bool allOfType = false);
		//��������� ������ �� �������. ����� ��������� ����������� ������� ���������� �� ������� ��� ���������.
		virtual bool TickUpdate(long long maxMillis = INFINITE_TIME);

	private:
		explicit EventManager();
		virtual ~EventManager();

	private:
		EventListenerMap m_eventListeners;
		EventQueue m_queue[EVENTMANAGER_NUM_QUEUES];
		int m_activeQueue;
	};

}