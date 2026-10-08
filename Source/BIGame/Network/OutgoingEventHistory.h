#pragma once

#include <deque>

#include "../../../BIEngine/EventManager/EventManager.h"

// Retains sent events until the server confirms the corresponding sequence.
class OutgoingEventHistory {
public:
   void Record(uint32_t sequence, BIEngine::IEventDataPtr event);
   void Acknowledge(uint32_t lastProcessedSequence);
   void Clear();

private:
   struct Entry {
      uint32_t sequence;
      BIEngine::IEventDataPtr event;
   };
   std::deque<Entry> m_pending;
};
