#include "EventManager.h"

#include <cassert>
#include <chrono>
#include <iostream>

#include "../Utilities/Logger.h"

namespace BIEngine
{

	static EventManager* g_pEventMgr = nullptr;
	EventFactory g_eventFactory;

	EventManager* EventManager::Get()
	{
		if (!g_pEventMgr)
			g_pEventMgr = new EventManager();

		assert(g_pEventMgr);
		return g_pEventMgr;
	}

	EventManager::EventManager()
		: m_eventListeners()
		, m_queue()
		, m_activeQueue(0)
	{
		if (g_pEventMgr)
		{
			Logger::WriteLog(Logger::LogType::ERROR, "Attempting to create two global event managers! The old one will be destroyed and overwritten with this one.\n");
			delete g_pEventMgr;
		}

		g_pEventMgr = this;
	}

	EventManager::~EventManager()
	{
		if (g_pEventMgr == this)
		{
			delete  g_pEventMgr;
			g_pEventMgr = nullptr;
		}
	}

	bool EventManager::AddListener(const EventListenerDelegate& eventDelegate, const EventType& type)
	{
		auto& eventListenerList = m_eventListeners[type];
		for (auto delegate : eventListenerList)
		{
			if (eventDelegate == delegate)
			{
				Logger::WriteLog(Logger::LogType::WARNING, "Attempting to double-register a delegate");
				return false;
			}
		}

		eventListenerList.push_back(eventDelegate);
		return true;
	}


	bool EventManager::RemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type)
	{
		bool success = false;

		auto findIt = m_eventListeners.find(type);
		if (findIt != m_eventListeners.end())
		{
			EventListenerList& listeners = findIt->second;
			for (auto itr = listeners.begin(); itr != listeners.end(); ++itr)
			{
				if (eventDelegate == (*itr))
				{
					listeners.erase(itr);
					success = true;
					break;
				}
			}
		}

		return success;
	}

	bool EventManager::TriggerEvent(const IEventDataPtr& pEvent) const
	{
		bool processed = false;

		auto findIt = m_eventListeners.find(pEvent->GetEventType());
		if (findIt != m_eventListeners.end())
		{
			const EventListenerList& eventListenerList = findIt->second;
			for (auto itr = eventListenerList.cbegin(); itr != eventListenerList.cend(); ++itr)
			{
				EventListenerDelegate listener = (*itr);
				listener(pEvent);
				processed = true;
			}
		}

		return processed;
	}

	bool EventManager::QueueEvent(const IEventDataPtr& pEvent)
	{
		assert(m_activeQueue >= 0);
		assert(m_activeQueue < EVENTMANAGER_NUM_QUEUES);

		auto findIt = m_eventListeners.find(pEvent->GetEventType());
		if (findIt != m_eventListeners.end())
		{
			m_queue[m_activeQueue].push_back(pEvent);
			return true;
		}

		return false;
	}


	bool EventManager::AbortEvent(const EventType& inType, bool allOfType)
	{
		assert(m_activeQueue >= 0);
		assert(m_activeQueue < EVENTMANAGER_NUM_QUEUES);

		bool success = false;
		auto findItr = m_eventListeners.find(inType);

		if (findItr != m_eventListeners.end())
		{
			EventQueue& eventQueue = m_queue[m_activeQueue];
			for (auto itr = eventQueue.begin(); itr != eventQueue.end(); ++itr)
			{
				if ((*itr)->GetEventType() == inType)
				{
					itr = eventQueue.erase(itr);
					success = true;
					if (!allOfType && itr == eventQueue.end())
						break;
				}
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
		m_queue[m_activeQueue].clear();

		while (!m_queue[queueToProcess].empty())
		{
			IEventDataPtr pEvent = m_queue[queueToProcess].front();
			m_queue[queueToProcess].pop_front();

			const EventType& eventType = pEvent->GetEventType();

			auto findIt = m_eventListeners.find(eventType);
			if (findIt != m_eventListeners.end())
			{
				const EventListenerList& eventListeners = findIt->second;

				for (auto it = eventListeners.begin(); it != eventListeners.end(); ++it)
				{
					EventListenerDelegate listener = (*it);
					listener(pEvent);
				}
			}

			currMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			if (maxMillis != EventManager::INFINITE_TIME && currMs >= maxMs)
			{
				Logger::WriteLog(Logger::LogType::MESSAGE, "Aborting event processing: time ran out");
				break;
			}
		}

		bool queueFlushed = m_queue[queueToProcess].empty();
		if (!queueFlushed)
		{
			while (!m_queue[queueToProcess].empty())
			{
				IEventDataPtr pEvent = m_queue[queueToProcess].back();
				m_queue[queueToProcess].pop_back();
				m_queue[m_activeQueue].push_front(pEvent);
			}
		}

		return queueFlushed;
	}

}