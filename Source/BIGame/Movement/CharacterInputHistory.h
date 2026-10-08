#pragma once

#include <deque>

#include "MovementTypes.h"

// Local fixed-step inputs awaiting confirmation by an authoritative snapshot.
class CharacterInputHistory {
public:
   void Record(const CharacterInputCommand& command);
   bool Acknowledge(uint32_t lastProcessedSequence);
   void Clear();

   const std::deque<CharacterInputCommand>& Pending() const { return m_pending; }

private:
   std::deque<CharacterInputCommand> m_pending;
   uint32_t m_lastRecordedSequence = 0;
   uint32_t m_lastAcknowledgedSequence = 0;
};
