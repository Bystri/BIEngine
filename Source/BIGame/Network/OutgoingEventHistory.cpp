#include "OutgoingEventHistory.h"

#include <utility>

void OutgoingEventHistory::Record(uint32_t sequence, BIEngine::IEventDataPtr event)
{
   m_pending.push_back(Entry{sequence, std::move(event)});
}

void OutgoingEventHistory::Acknowledge(uint32_t lastProcessedSequence)
{
   while (!m_pending.empty() && m_pending.front().sequence <= lastProcessedSequence) {
      m_pending.pop_front();
   }
}

void OutgoingEventHistory::Clear()
{
   m_pending.clear();
}
